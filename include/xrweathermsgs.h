#include "xrweather.h"

char *xrweatherwindmsgs[RAIN_MSGS][RAIN_MSGS] =
  {
    /* Old is 0 */
    {
	"",
	"A light breeze starts to blow.\n",
	"A fresh breeze begins to blow.\n",
	"A strong breeze starts to blow.\n",
	"",
	"",
	""
    },

    /* Old is 1 */
    {
	"The breeze dies away until the air is still.\n",
	"",
	"The breeze gets a little stronger.\n",
	"The wind freshens to a strong breeze.\n",
	"The wind quickly escalates to a light gale.\n",
	"",
	""
    },

    /* Old is 2 */
    {
	"The breeze gradually dies away until all is calm.\n",
	"The wind drops to a very light breeze.\n",
	"",
	"The wind grows a little stronger.\n",
	"The breeze freshens to a light gale.\n",
	"",
	""
    },

    /* Old is 3 */
    {
	"The breeze dies away to nothing.  All is now calm.\n",
	"The wind dies down to a light breeze.\n",
	"The breeze diminishes in strength a little.\n",
	"",
	"The wind increases to a light gale.\n",
	"The breeze strengthens suddenly into a strong gale.\n",
	""
    },

    /* Old is 4 */
    {
	"",
	"The light gale suddenly dies down to a very light breeze.\n",
	"The light gale drops to a fresh breeze.\n",
	"The gale decreases to a strong breeze.\n",
	"",
	"The gale grows stronger.\n",
	"The gale escalates into storm-force winds with frightening speed!"
    },

    /* Old is 5 */
    {
	"",
	"",
	"",
	"The gale quickly dies down to a strong breeze.\n",
	"The gale lessens somewhat.\n",
	"",
	"You are almost swept away as the gale blows up into storm-force winds.\n"
    },

    /* Old is 6 */
    {
	"",
	"",
	"",
	"",
	"The stormy winds die down to a light gale.\n",
	"The stormy winds die down a little.\n",
	""
    }
};

char *xrweatherstartmsgs[RAIN_MSGS][RAIN_MSGS][2] =
  {
    {
	/* 0/0 */
	{
	"",
	""
	},

	/* 0/1 */
	{
	"You start to notice a couple of light raindrops falling from the sky.\n",
	"You start to notice a few snowflakes fall from the sky.\n"
	},
	
	{
	"Rain starts to fall as a light drizzle.\n",
	"You start to notice a few snowflakes fall from the sky.\n"
	},

	{
	"A light rain begins to fall from the heavens.\n",
	"Snow begins to fall from the clouds.\n"
	},

	{
	"The heavens open and it begins to rain.\n",
	"Snow begins to fall from the clouds.\n"
	},

	{
	"A heavy rain begins to fall from the dark clouds overhead.\n",
	"Snow begins to fall thickly from the cloudy sky.\n"
	},

	{
	"The water-logged clouds above you yield their load as a downpour commences.\n",
	"Snow begins to fall thickly from the cloudy sky.\n"
	},
	
	{
	"You are instantly drenched as torrents of water begin to fall from the sky.\n",
	"Snow begins to fall thickly from the cloudy sky.\n"
	}
    },

    /* Old rain is 1 */
    {
	{
	"No more raindrops are falling.\n",
	"The light snowfall ceases.\n"
	},

	{
	"",
	""
	},

	{
	"The rain increases to a light drizzle.\n",
	""
	},

	{
	"The rain begins to come thicker and faster.\n",
	"The snow starts to fall a little more thickly.\n"
	},

	{
	"The rainfall suddenly grows more intense.\n",
	"The snow starts to fall a little more thickly.\n"
	},

	{
	"The rain suddenly begins to fall much more heavily.\n",
	"The snow begins to come down much thicker and faster.\n"
	},

	{
	"In a matter of seconds the rain intensifies to a downpour.\n",
	"The snow begins to come down much thicker and faster.\n"
	},

	{
	"Without warning the light sprinkling of rain becomes a torrential downpour.\n",
	"The snow begins to come down much thicker and faster.\n"
	}
    },

    /* Old value is 2 */
    {
	{
	"The drizzle stops.\n",
	"The light snowfall ceases.\n"
	},

	{
	"The drizzle decreases to just a few spots.\n",
	""
	},

	{
	"",
	""
	},

	{
	"The rain starts to fall more steadily.\n",
	"The snow starts to fall a little more thickly.\n"
	},

	{
	"The rainfall grows heavier.\n",
	"The snow starts to fall a little more thickly.\n"
	},

	{
	"The drizzle intensifies to a heavy rain.\n",
	"The snow begins to come down much thicker and faster.\n"
	},

	{
	"The drizzle becomes a downpour.\n",
	"The snow begins to come down much thicker and faster.\n"
	},

	{
	"The light drizzle suddenly turns into torrential sheets of rain.\n",
	"The snow begins to come down much thicker and faster.\n"
	}
    },

    /* Old is 3 */
    {
	{
	"The light rain stops falling for a time.\n",
	"The snow stops falling.\n"
	},

	{
	"The light rain all but stops.\n",
	"The falling snow diminishes to a few flakes.\n"
	},

	{
	"The light rain becomes little more than a drizzle.\n",
	"The falling snow diminishes to a few flakes.\n"
	},

	{
	"",
	""
	},

	{
	"The rain begins to fall a little harder.\n",
	""
	},

	{
	"The light rainfall grows much heavier.\n",
	"The snowfall becomes much heavier.\n"
	},

	{
	"The rain becomes a downpour.\n",
	"The snowfall becomes much heavier.\n"
	},

	{
	"The light rain turns into a monsoon-like downpour.\n",
	"The snowfall becomes much heavier.\n"
	}
    },

    /* Old value is 4 */
    {
	{
	"The rain stops for the moment.\n",
	"The snow stops falling.\n"
	},

	{
	"The rain decreases to a few light spots.\n",
	"The falling snow diminishes to a few flakes.\n"
	},

	{
	"The rain lessens to a light drizzle.\n",
	"The falling snow diminishes to a few flakes.\n"
	},

	{
	"The rain eases off a little.\n",
	""
	},

	{
	"",
	""
	},

	{
	"The rainfall increases in quantity.\n",
	"The snowfall becomes much heavier.\n"
	},

	{
	"The rain becomes a downpour.\n",
	"The snowfall becomes much heavier.\n"
	},

	{
	"The rainfall suddenly becomes torrential.\n",
	"The snowfall becomes much heavier.\n"
	}
    },

    /* Old is 5 */
    {
	{
	"The heavy rain suddenly stops falling.\n",
	"The snow suddenly stops falling.\n"
	},

	{
	"The rain all but stops, one or two spots still falling occasionally.\n",
	"The snow starts to fall a lot less thickly.\n"
	},

	{
	"The rain decreases to a drizzle.\n",
	"The snow starts to fall a lot less thickly.\n"
	},

	{
	"The rain gets a little lighter.\n",
	"The snowfall becomes a little lighter.\n"
	},

	{
	"The rain starts to get a little less heavy.\n",
	"The snowfall becomes a little lighter.\n"
	},

	{
	"",
	""
	},

	{
	"The heavy rain grows even heavier.\n",
	""
	},

	{
	"The heavy rainfall becomes a torrential downpour.\n",
	""
	}
    },

    /* Old is 6 */
    {
	{
	"The downpour ceases, leaving the air fresh and clean.\n",
	"The snow suddenly stops falling.\n"
	},

	{
	"The downpour ceases, and only a few occasional spots of rain now fall.\n",
	"The snow starts to fall a lot less thickly.\n"
	},

	{
	"The downpour suddenly slackens off to a light drizzle.\n",
	"The snow starts to fall a lot less thickly.\n"
	},

	{
	"The downpour decreases to a gentle rain.\n",
	"The snowfall becomes a little lighter.\n"
	},

	{
	"The downpour eases off somewhat.\n",
	"The snowfall becomes a little lighter.\n"
	},

	{
	"The downpour slackens off a little.\n",
	""
	},

	{
	"",
	""
	},

	{
	"The downpour becomes even more torrential.\n",
	""
	}
    },

    /* Old is 7 */
    {
	{
	"The torrential downpour abruptly ceases.\n",
	"The snow suddenly stops falling.\n"
	},

	{
	"The torrential rain suddenly ceases, leaving only one or two specks of rain.\n",
	"The snow starts to fall a lot less thickly.\n"
	},

	{
	"The torrential rain suddenly decreases to a light drizzle.\n",
	"The snow starts to fall a lot less thickly.\n"
	},

	{
	"The torrential rain grows suddenly much less severe.\n",
	"The snowfall becomes a little lighter.\n"
	},

	{
	"The downpour slackens off to a more moderate rainfall.\n",
	"The snowfall becomes a little lighter.\n"
	},

	{
	"The downpour eases off considerably.\n",
	""
	},

	{
	"The downpour becomes a little less intense.\n",
	""
	},

	{
	"",
	""
	}
    }
};

char *xrweathermsgs[WIND_MSGS][RAIN_MSGS][3] = 
  {
    /* Wind is 0 */
    {
        {
	"",
	"",
	""
        },

        {
	"One or two spots of rain drift down from the sky to alight on your face.\n",
	"A few snowflakes drift down from the cloudy sky.\n",
	"One or two spots of rain drift down from the sky to alight on your face.\n"
        },

        {
	"A light drizzle is falling, making your skin clammy and your clothes damp.\n",
	"A few snowflakes drift down from the cloudy sky.\n",
	"A light drizzle is falling, making your skin clammy and your clothes damp.\n"
        },

	{
	"A soft rain is falling from the overcast sky.\n",
	"A gentle snow is falling steadily from the heavens.\n",
	"A soft rain is falling from the overcast sky.\n"
	},
	
	{
	"A steady rain is turning the ground into slippery mud.\n",
	"A gentle snow is falling steadily from the heavens.\n",
	"A steady rain is falling, disturbing the smoothness of the water's surface.\n"
	},

	{
	"Puddles are forming on the ground as a heavy rain descends from the heavens.\n",
	"Snow is falling thickly.\n",
	"Large raindrops splash into the water as heavy rain descends from the sky.\n"
	},

	{
	"Torrential rain is falling, causing rivulets and streams to form at your feet.\n",
	"Snow is falling thickly.\n",
	"Torrents of rain are falling into the water around you.\n"
	},

	{
	"Sheets of rain are descending in a monsoon-like downpour.\n",
	"Snow is falling thickly.\n",
	"Sheets of rain are descending in a monsoon-like downpour.\n"
	}
    },

    /* Wind is 1 */
    {

	{
	"A hint of a breeze wafts against your face.\n",
	"A hint of a breeze wafts against your face.\n",
	"A hint of a breeze wafts against your face.\n"
	},

	{
	"A gentle breath of wind toys with the occasional speck of rain.\n",
	"A few snowflakes are being blown about in the breeze.\n",
	"A gentle breath of wind toys with the occasional speck of rain.\n"
	},

	{
	"A damp veil of rain drifts along with the light breeze.\n",
	"A few snowflakes are being blown about in the breeze.\n",
	"A damp veil of rain drifts along with the light breeze.\n"
	},

	{
	"A soft rain is falling as a breath of wind stirs the air.\n",
	"The breeze is making the snowflakes swirl about as they fall.\n",
	"A soft rain is falling as a breath of wind stirs the air.\n",
	},

	{
	"A gentle breeze drifts across the land as a steady rain falls from the sky.\n",
	"The breeze is making the snowflakes swirl about as they fall.\n",
	"A gentle breeze drifts over the water as a steady rain falls from the sky.\n"
	},

	{
	"A heavy rain is falling as a light breeze freshens the air.\n",
	"The thickly-falling snow is scarcely affected by the breeze.\n",
	"A heavy rain is falling as a light breeze freshens the air.\n"
	},

	{
	"A cloudburst is drenching the land as the air stirs in a light breeze.\n",
	"The thickly-falling snow is scarcely affected by the breeze.\n",
	"A gentle breeze blows as a cloudburst pours torrents of rain upon the water.\n"
	},

	{
	"The ground is awash as sheets of rain descend amid a slight breath of wind.\n",
	"The thickly-falling snow is scarcely affected by the breeze.\n",
	"A breath of wind blows as sheets of rain descend from the heavens into the sea.\n"
	}
    },
    
    /* Wind is 2 */
    {
	
	{
	"A fresh breeze is blowing.\n",
	"A fresh breeze is blowing.\n",
	"A fresh breeze is blowing.\n"
	},

	{
	"A fresh breeze blows occasional droplets of rain into your face.\n",
	"A few snowflakes are being blown about in the breeze.\n",
	"A fresh breeze blows occasional droplets of rain into your face.\n"
	},

	{
	"An unpleasant drizzle is being driven before a light wind.\n",
	"A few snowflakes are being blown about in the breeze.\n",
	"An unpleasant drizzle is being driven before a light wind.\n"
	},

	{
	"A light fall of rain is being whipped along by a fresh breeze.\n",
	"The breeze is making the snowflakes swirl about as they fall.\n",
	"A light fall of rain is being whipped along by a fresh breeze.\n"
	},

	{
	"A fresh breeze is blowing as rain falls from the sky.\n",
	"The breeze is making the snowflakes swirl about as they fall.\n",
	"A fresh breeze is blowing as rain falls from the sky.\n"
	},

	{
	"Heavy rain is pouring down around you while a fresh wind blows.\n",
	"The wind blows the snow about as it falls thickly from the cloudy sky.\n",
	"Heavy rain is pouring down around you while a fresh wind blows.\n",
	},

	{
	"A light wind is blowing as torrential rain pours from the skies.\n",
	"The wind blows the snow about as it falls thickly from the cloudy sky.\n",
	"A light wind is blowing as torrential rain pours from the skies.\n"
	},

	{
	"An incredible downpour of rain is descending as a fresh wind blows.\n",
	"The wind blows the snow about as it falls thickly from the cloudy sky.\n",
	"An incredible downpour of rain is descending as a fresh wind blows.\n"
	}
    },

    /* Wind is 3 */
    {
	{
	"A fresh wind tugs at your clothing.\n",
	"A fresh wind tugs at your clothing.\n",
	"A fresh wind tugs at your clothing.\n"
	},

	{
	"A strong breeze whips a couple of spots of rain in your direction.\n",
	"Snowflakes are flying about in the wind.\n",
	"A strong breeze whips a couple of spots of rain in your direction.\n"
	},

	{
	"Tiny raindrops sting your face as a strong breeze blows them at you.\n",
	"Snowflakes are flying about in the wind.\n",
	"Tiny raindrops sting your face as a strong breeze blows them at you.\n"
	},

	{
	"A light rainfall descends at an angle, driven by a strong breeze.\n",
	"The falling snow is being blown about by the wind.\n",
	"A light rainfall descends at an angle, driven by a strong breeze.\n"
	},

	{
	"A steady rain is being whipped about by a fresh wind.\n",
	"The falling snow is being blown about by the wind.\n",
	"A steady rain is being whipped about by a fresh wind.\n"
	},

	{
	"A strong breeze cools the air as a heavy rainfall descends.\n",
	"The wind blows the snow about as it falls thickly from the cloudy sky.\n",
	"A strong breeze cools the air as a heavy rainfall descends.\n"
	},

	{
	"A torrent of rain is pouring down amid a fresh wind.\n",
	"The wind blows the snow about as it falls thickly from the cloudy sky.\n",
	"A torrent of rain is pouring down amid a fresh wind.\n"
	},

	{
	"Sheets of monsoon-like rain are lashing down as a strong breeze blows.\n",
	"The wind blows the snow about as it falls thickly from the cloudy sky.\n",
	"Sheets of monsoon-like rain are lashing down as a strong breeze blows.\n"
	}
    },

    /* Wind is 4 */
    {
	{
	"A light gale is blowing across the land.\n",
	"A light gale is blowing across the land.\n",
	"A light gale is blowing over the water, whipping off the tops of the waves.\n"
	},

	{
	"A strong wind blows one or two raindrops into your face.\n",
	"Snowflakes are flying about in the wind.\n",
	"A strong wind blows one or two raindrops into your face.\n"
	},

	{
	"A light drizzle is being swept horizontally in the strong wind.\n",
	"Snowflakes are flying about in the wind.\n",
	"A light drizzle is being swept horizontally in the strong wind.\n"
	},

	{
	"A light rain is falling amid strong winds.\n",
	"The wind is blowing the falling snow horizontally.\n",
	"A light rain is falling amid strong winds.\n"
	},

	{
	"A light gale is blowing as the rain falls steadily.\n",
	"The wind is blowing the falling snow horizontally.\n",
	"A light gale is blowing as the rain falls steadily.\n"
	},

	{
	"A strong wind whips at your clothing as heavy rain descends from the sky.\n",
	"Visibility is poor as thick snow is driven into your face by the wind.\n",
	"A strong wind whips at your clothing as heavy rain descends from the sky.\n"
	},

	{
	"The strong winds and torrential rain are putting a real crimp in your day.\n",
	"Visibility is poor as thick snow is driven into your face by the wind.\n",
	"The strong winds and torrential rain are putting a real crimp in your day.\n"
	},

	{
	"A light gale blows as the monsoon-like rains pour down on you.\n",
	"Visibility is poor as thick snow is driven into your face by the wind.\n",
	"A light gale blows as the monsoon-like rains pour down on you.\n"
	}
    },

    /* Wind is 5 */
    {
	{
	"A strong gale is sweeping through the land.\n",
	"A strong gale is sweeping through the land.\n",
	"A strong gale is whipping up the water into tall waves.\n"
	},

	{
	"A couple of specks of rain land on your face as a strong gale blows.\n",
	"The wind blasts tiny snow particles into your face.\n",
	"A couple of specks of rain land on your face as a strong gale blows.\n",
	},

	{
	"The gale blasts a wetting drizzle into your face.\n",
	"The wind blasts tiny snow particles into your face.\n",
	"The gale blasts a wetting drizzle into your face.\n"
	},

	{
	"A strong gale ushers along the falling raindrops.\n",
	"A gale sends the falling snow hurtling by at high speed.\n",
	"A strong gale ushers along the falling raindrops.\n"
	},

	{
	"Powerful winds blast the falling rain at you.\n",
	"A gale sends the falling snow hurtling by at high speed.\n",
	"Powerful winds blast the falling rain at you.\n"
	},

	{
	"The rain is falling heavily as a strong gale blows.\n",
	"You are in the midst of a blizzard as the gale pelts you with thick snow.\n",
	"The rain is falling heavily as a strong gale blows.\n"
	},

	{
	"Stormy conditions prevail as lashing rains and a strong gale blast at you.\n",
	"You are in the midst of a blizzard as the gale pelts you with thick snow.\n",
	"Stormy conditions prevail as lashing rains and a strong gale blast at you.\n"
	},

	{
	"An intense storm is raging as monsoon-like rains mix with a strong gale.\n",
	"You are in the midst of a blizzard as the gale pelts you with thick snow.\n",
	"An intense storm is raging as monsoon-like rains mix with a strong gale.\n"
	}
    },

    /* Wind is 6 */
    {
	{
	"Storm force winds wreak havoc across the countryside.\n",
	"Storm force winds wreak havoc across the countryside.\n",
	"Storm force winds blow across the water, throwing up dangerously high waves.\n"
	},

	{
	"The occasional raindrop goes unnoticed as violent storm winds prevail.\n",
	"You are being pelted with high velocity snowflakes by the ferocious winds.\n",
	"The occasional raindrop goes unnoticed as violent storm winds prevail.\n"
	},

	{
	"Violent winds blast a stinging drizzle into your face.\n",
	"You are being pelted with high velocity snowflakes by the ferocious winds.\n",
	"Violent winds blast a stinging drizzle into your face.\n"
	},

	{
	"A light rain is swept across the land by storm-force winds.\n",
	"A blizzard rages around you as the storm winds blast the falling snow at you.\n",
	"Amid the crashing waves you hardly notice the rain as the high winds blow.\n"
	},

	{
	"Strong storm winds drive the falling rain across the land.\n",
	"A blizzard rages around you as the storm winds blast the falling snow at you.\n",
	"You are soaked by rain and spray from the waves as storm-force winds rage.\n"
	},

	{
	"A storm rages as violent winds combine with the heavy rain.\n",
	"A fierce blizzard rages, almost choking you with a bombardment of thick snow.\n",
	"A storm rages as violent winds combine with the heavy rain.\n"
	},

	{
	"A violent storm rages with terrific winds and lashing rain.\n",
	"A fierce blizzard rages, almost choking you with a bombardment of thick snow.\n",
	"A violent storm rages with terrific winds and lashing rain.\n"
	},

	{
	"Torrential sheets of rain are driven sideways by violent storm winds.\n",
	"A fierce blizzard rages, almost choking you with a bombardment of thick snow.\n",
	"Torrential sheets of rain are driven sideways by violent storm winds.\n"
	} 
    }
};
 
