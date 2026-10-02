#ifndef XR_WEATHER
#define XR_WEATHER

void xrchange_weather( );
void weathercom (void);
void start_weather( int weather );
void boot_weather( );
int get_temp( int location );
Boolean is_daytime( int location );
void show_weather (void);
int rain2num( int plr, int value );
int wind2num( int value );
void rainmsgs( int oldvalue );
void windmsgs( int oldwind );
char *raintest_func( int plr, int arg, char *msg );
char *windtest_func( int plr, int arg, char *msg );

/* Defines for start_weather */
#define WEATHER_SUNNY 0
#define WEATHER_CLOUDY 1
#define WEATHER_RAIN 2
#define WEATHER_HURRICANE 3

/* Changing these changes the size of the structure in xrweathermsgs.h */
#define RAIN_MSGS 8
#define WIND_MSGS 7
extern char *xrweathermsgs[WIND_MSGS][RAIN_MSGS][3];

#endif
