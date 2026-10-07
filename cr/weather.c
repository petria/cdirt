#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "kernel.h"
#include "sendsys.h"
#include "climate.h"
#include "pflags.h"
#include "sflags.h"
#include "lflags.h"
#include "bprintf.h"
#include "parse.h" 
#include "rooms.h"
#include "xrweathermsgs.h"
#include "log.h"

#ifdef XR_WEATHER

Boolean is_daytime( int location )
  {
  time_t t_time;
  struct tm *the_time;

  time (&t_time);
  the_time = localtime (&t_time);

  if( 5 < the_time->tm_hour && the_time->tm_hour < 17 )
    return True;

  return False;
  
}

/*
 * Function used to display weather to mynum when entering a room.
 */

void show_weather (void)
  {
  int temp;
  int wind;
  int rain;

  if( !ltstflg( ploc (mynum), LFL_OUTDOORS ) )
    return;

  /* Determine the wind array position. */
  wind = wind2num( global_weather->windspeed );

  /* Determine the Rain array position. */
  rain = rain2num( mynum, global_weather->rain );
 
  /* Determine the Location/temp array position. */
  temp = get_temp( ploc(mynum) );
  if( temp < 40 )
    temp = 1; /* Snowing land & water */
  else if( ltstflg( ploc(mynum), LFL_ON_WATER ) ) 
    temp = 2; /* On Water */
  else
    temp = 0; /* On Land */

  /* Adjust for only wind messages (not raining. :)) */
  if( rain == 0 )
    temp = 0;

  /* Print the message */

  if( global_weather->sunny ) {
    if( is_daytime( ploc(mynum) ) )
      bprintf( "The &+Ysun&N is shining.\n" );
    else
      bprintf( "The &+Ymoonlight&N shines down from above.\n" );
  }

  bprintf( "%s", xrweathermsgs[wind][rain][temp] );
  return;
 
}

/*
 * Game command to tell us what the weather is like.  
 */

void weathercom (void)
  {
  int room;

  if (!ptstflg (mynum, PFL_WEATHER)) {
    erreval ();
    return;
  }

  if( ( room = getroomnum( ) ) == 0 )
    room = ploc( mynum );

  if( plev(mynum) >= LVL_SHALAFI ) {
    if( !strcmp( wordbuf, "sunny" ) ) {
      start_weather( WEATHER_SUNNY );
      return;
    }
    else if( !strcmp(wordbuf, "cloudy") ) {
      start_weather( WEATHER_CLOUDY );
      return;
    }
    else if( !strcmp(wordbuf, "rainy" ) ) {
      if( global_weather->sunny )
        start_weather( WEATHER_CLOUDY );
      else
        start_weather( WEATHER_RAIN );
      return;
    }
  }

  bprintf( "&+CCurrent weather conditions for &+B%s&+C:\n", 
           sdesc( room ) );
  bprintf( "&+GTemperature: &+W%d degrees.\n", get_temp( room ) );
  bprintf( "&+GLatitude:    &+W%d degrees.\n", zlatitude( lzone( room ) ) );
  bprintf( "&+GAltitude:    &+W%d feet.\n", laltitude( room ) );
  bprintf( "&+GRainfall:    &+W%d mm per year.\n", zrainfall( lzone( room ) ) );

  bprintf( "\n&+CGlobal weather conditions:\n" );
  bprintf( "&+GWeather           : " );
    if( !global_weather->sunny )
        bprintf( "&+LCloudy\n" );
    else if( is_daytime(room) )
      bprintf( "&+YSunny\n" );
    else
      bprintf( "&+WMoonlit\n" );
  bprintf( "&+GWind Speed        : &+W%d\n", global_weather->windspeed );
  if( plev(mynum) >= LVL_SHALAFI )
    {
    bprintf( "&+GRain              : &+W%d\n", global_weather->rain );
    bprintf( "&+GWeather Timer     : &+W%d\n", global_weather->timer );
    if( global_weather->sunny == False ) {
      bprintf( "&+GEnd of Standby    : &+W%d\n", global_weather->standby );
      bprintf( "&+GEnd of Decline    : &+W%d\n", global_weather->decline );
      bprintf( "&+GEnd of Steady     : &+W%d\n", global_weather->steady );
    }
    bprintf( "&+GWeather Until     : &+W%d\n", global_weather->weatherlen );
    if( global_weather->sunny == False )
      bprintf( "&+GBottom            : &+W%d\n", global_weather->bottom );
  }
  return;
  
}

/*
 * NASTY function to get the temperature in a given room.  It's based on:
 *
 * 1) Time Of The Year
 * 2) Latitude
 * 3) Altitude
 * 4) Rainfall 
 * 5) Wind Speed
 *
 * Don't even try to comprehend it.  :)
 */

int get_temp( int location )
  {
  int temperature;
  time_t t_time;
  struct tm *the_time;

  time (&t_time);
  the_time = localtime (&t_time);

  temperature = 125+global_weather->sunny*10-
    pow(zlatitude(lzone(location)),2)/75-
    laltitude(location)/250-
    abs((the_time->tm_mon-6))*7*zlatitude(lzone(location))/90-
    abs(global_weather->windspeed)/4-
    (int)((double)10*log10((double)(1+zrainfall(lzone(location)))) *
          (double)((double)1-((double)zlatitude(lzone(location))/(double)90)))-
    (int)((double).6 * 
          ((double)7 -log10((double)(1+zrainfall(lzone(location)))))*
          abs((the_time->tm_hour-12))* 
          (double)(110-zlatitude(lzone(location)))/(double)90);

  return temperature; 
}

/*
 * Change the wind speed once every two minutes by +/- 16 usually 
 * much smaller values than that, though.  :)
 */

void update_weather( )
  {
  int variance, oldwind;

  /* Check to run weather routines every two minutes */
  global_weather->counter += 1;
  if ( global_weather->counter != 60 ) 
     return;

  global_weather->counter = 0;
  global_weather->timer += 1;

  /* Wind variance stuff */
  oldwind = global_weather->windspeed;

  variance = (((rand()%2)?1:-1)* 
             (rnd(8)*rnd(8)*rnd(8)*rnd(8)/256));
  if ( variance > 0 && global_weather->windspeed >= 25 &&
       global_weather->windspeed < 80 && ( rnd(20) == 1 ) )
     variance = -variance;
  if(((global_weather->timer - global_weather->last_hurricane ) < 120 ) &&  
          variance > 0 && global_weather->windspeed > 75 )
     variance = -variance;
  global_weather->windspeed += variance;
  if( global_weather->windspeed <= -10 ) 
    global_weather->windspeed = -10;
  else if( global_weather->windspeed >= 100 ) {
    start_weather( WEATHER_HURRICANE );
  }
  windmsgs( oldwind );

  /*
   * Rain stuff.  Check to see if it's time to switch from rainy<->sunny 
   */
  if( (global_weather->weatherlen - global_weather->timer) <= 0 )
    {
    if( global_weather->sunny )
      start_weather( WEATHER_CLOUDY );
    else
      start_weather( WEATHER_SUNNY );
  }

  /* 
   * Dont' update rain info if it's:
   *
   * a) sunny
   * b) waiting to start to decline to steady
   * c) at steady
   */
  if( global_weather->sunny || 
    (global_weather->timer < global_weather->standby )||
    (global_weather->decline < global_weather->timer &&
     global_weather->timer < global_weather->steady)) 
    return;

  /* Time to start a new cycle? */
  if( global_weather->timer > global_weather->duration ) {
    start_weather( WEATHER_RAIN );
    return;
  }

  /* We're declining. */
  if(global_weather->standby < global_weather->timer < global_weather->decline)
    {
    int old_value = global_weather->rain;
    global_weather->rain = 10000 - (int)((double)(10000 - global_weather->bottom) *
                           (double)(global_weather->timer - global_weather->standby) /
                           (double)(global_weather->decline - global_weather->standby));            
    rainmsgs( old_value );
    return;
  } 

  /* Inclining */
  if(global_weather->duration > global_weather->timer > global_weather->steady)
    {
    int old_value = global_weather->rain;
    global_weather->rain = (int)( (double)(global_weather->timer - global_weather->steady) * 
                           (double)(10000 - global_weather->bottom ) / 
                           (double)(global_weather->duration - global_weather->steady) + 
                           (double)global_weather->bottom);
    rainmsgs( old_value );
    return;
  }

  if( global_weather->timer >= global_weather->duration )
    {
    if((global_weather->weatherlen - global_weather->timer) > 5)
      start_weather( WEATHER_RAIN );
    return;
  }
  return;
}

/*
 * Routine to boot weather sequences.
 */

void boot_weather( )
  {
  FILE *weatherfile;

  global_weather->windspeed = 0;
  global_weather->timer = 0;
  global_weather->counter = 0;
  global_weather->last_hurricane = 0;
  global_weather->bottom = 0;
  global_weather->duration = 0;
  global_weather->decline = 0;
  global_weather->steady = 0;

  if( ( weatherfile = FOPEN( WEATHER_FILE, "r" )) == NULL ) {
    start_weather( WEATHER_SUNNY );
    return;
  }

  fscanf( weatherfile, "Sunny:      %d\n", (int *) &global_weather->sunny );
  fscanf( weatherfile, "Windspeed:  %d\n", &global_weather->windspeed );
  fscanf( weatherfile, "Rain:       %d\n", &global_weather->rain );
  fscanf( weatherfile, "Timer:      %d\n", &global_weather->timer );
  fscanf( weatherfile, "Counter:    %d\n", &global_weather->counter );
  fscanf( weatherfile, "LHurricane: %d\n", &global_weather->last_hurricane );
  fscanf( weatherfile, "WeatherLen: %d\n", &global_weather->weatherlen );
  fscanf( weatherfile, "Bottom:     %d\n", &global_weather->bottom );
  fscanf( weatherfile, "Duration:   %d\n", &global_weather->duration );
  fscanf( weatherfile, "Standby:    %d\n", &global_weather->standby );
  fscanf( weatherfile, "Decline:    %d\n", &global_weather->decline );
  fscanf( weatherfile, "Steady:     %d\n", &global_weather->steady );

  FCLOSE( weatherfile );

}

void start_weather( int weather )
  {
  int steady;

  if( weather == WEATHER_SUNNY ) {
    global_weather->weatherlen = (int)((double)(rnd(2)*rnd(2)*rnd(2)*rnd(2)*
				 rnd(2)*rnd(2)*rnd(2)*rnd(2)*rnd(2)*rnd(2)*
				 rnd(2)*rnd(2)*rnd(2)*rnd(2))*((double).67+
                                 drand48()*(double)2/(double)3)/(double)2);
    global_weather->weatherlen += global_weather->timer;
    global_weather->sunny = True;
    global_weather->rain = 10000;
    return;
  }
  if( weather == WEATHER_CLOUDY ) {
    global_weather->sunny = False;
    global_weather->weatherlen = (int)((double)(rnd(4)*rnd(4)*rnd(4)*rnd(4)*
                                 rnd(4)*rnd(4)*rnd(4))*((double).8+drand48()*
				 (double)2/(double)5)/(double)2);
    global_weather->weatherlen += global_weather->timer;
    start_weather( WEATHER_RAIN );
    return;
  } 
  if( weather == WEATHER_RAIN ) {
    int raintime;

    /* Duration of rainy spell */
    global_weather->duration = 3+(int)(drand48()*
                                      (double)global_weather->weatherlen); 

    /* Time before it starts to decline. */
    global_weather->standby = 1+(int)((double)global_weather->duration*
                      drand48()/(double)2);
    if( global_weather->duration - global_weather->standby == 1 )
      global_weather->standby = 1;

    /* Length of the actual rain period */
    raintime = global_weather->duration - global_weather->standby;

    /* The time that it takes to go from 10000 to bottom */
    if( raintime == 2 )
      global_weather->decline = 1;
    else
      global_weather->decline = rnd((raintime/2));
    
    /* Time that the rain will stay at bottom */
    
    steady = raintime - global_weather->decline - 1;
    global_weather->steady = rnd(steady);

    /* The hardest that it will rain during this spell */
    if( global_weather->duration > 15 )
      global_weather->bottom = rnd(10)*rnd(10)*rnd(100);
    else
      global_weather->bottom = 10000-rnd(8500);

    /* 
     * Now the tricky part.  Convert all of these values to "ticks" WHEN
     * they will happen instead of times. :)  Also these times are when
     * the periods end instead of begin.
     */
    global_weather->standby += global_weather->timer;
    global_weather->decline += global_weather->standby;
    global_weather->steady += global_weather->decline;
    global_weather->duration += global_weather->timer;
    return;
  }
  if( weather == WEATHER_HURRICANE ) {
    /* Not implemented yet */
    mudlog( "Hurricane Started!!!" );
    global_weather->last_hurricane = global_weather->timer;
    global_weather->windspeed  = 100;
    return;
  }
  mudlog( "Unknown weather in start_weather %d!", weather );
  return;
}

/*
 * Sends a message to everyone if the weather changes in their room 
 */

void rainmsgs( int oldvalue ) {
  if( oldvalue == global_weather->rain )
    return;
  send_g_msg( DEST_ALL, raintest_func, oldvalue, NULL );
  return;
}

void windmsgs( int oldwind ) {
  if( wind2num( oldwind ) == wind2num( global_weather->windspeed ) )
    return;
  send_g_msg( DEST_ALL, windtest_func, oldwind, NULL );
  return;
}

char *windtest_func( int plr, intptr_t arg, char *msg ) {
  if( !ltstflg( ploc( plr ), LFL_OUTDOORS ) || ststflg( plr, SFL_NOWET ) ||
      ststflg( plr, SFL_QUIET ) )
    return NULL;

  return xrweatherwindmsgs[wind2num((int)arg)][wind2num(global_weather->windspeed)];
}

char * raintest_func( int plr, intptr_t arg, char *msg ) {
  if( !ltstflg( ploc( plr ), LFL_OUTDOORS ) || ststflg( plr, SFL_NOWET ) ||
      ststflg( plr, SFL_QUIET ) )
    return NULL;

  return xrweatherstartmsgs[rain2num( plr, (int)arg )][rain2num( plr, global_weather->rain )][((get_temp(ploc(plr)) < 40) ? 1 : 0)];
}

/* 
 * Function to convert actual rainfall values into a value used to access an 
 * array.  Returns a value from 0-7 depending on the rainfall of plr.
 */

int rain2num( int plr, int value ) {
  if( ( zrainfall( lzone(ploc(plr) ) ) - value ) > 2500 )
    return 7;
  else if( ( zrainfall( lzone(ploc(plr) ) ) - value ) > 1000 )
    return 6;
  else if( ( zrainfall( lzone(ploc(plr) ) ) - value ) > 500 )
    return 5;
  else if( ( zrainfall( lzone(ploc(plr) ) ) - value ) > 300 )
    return 4;
  else if( ( zrainfall( lzone(ploc(plr) ) ) - value ) > 120 )
    return 3;
  else if( ( zrainfall( lzone(ploc(plr) ) ) - value ) > 30 )
    return 2;
  else if( ( zrainfall( lzone(ploc(plr) ) ) - value ) > 0 )
    return 1;
  else
    return 0;
}

int wind2num( int value ) {
  if( abs( value ) > 84 )
    return 6;
  else if( abs( value ) > 59 )
    return 5;
  else if( abs( value ) > 39 )
    return 4;
  else if( abs( value ) > 24 )
    return 3;
  else if( abs( value ) > 10 )
    return 2;
  else if( abs( value ) > 0 )
    return 1;
  else
    return 0;
}
#endif /* XR_WEATHER */
