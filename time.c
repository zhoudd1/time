#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

struct tm {
    /*
     * the number of seconds after the minute, normally in the range
     * 0 to 59, but can be up to 60 to allow for leap seconds
     */
    int tm_sec;
    /* the number of minutes after the hour, in the range 0 to 59*/
    int tm_min;
    /* the number of hours past midnight, in the range 0 to 23 */
    int tm_hour;
    /* the day of the month, in the range 1 to 31 */
    int tm_mday;
    /* the number of months since January, in the range 0 to 11 */
    int tm_mon;
    /* the number of years since 1900 */
    long tm_year;
    /* the number of days since Sunday, in the range 0 to 6 */
    int tm_wday;
    /* the number of days since January 1, in the range 0 to 365 */
    int tm_yday;

    int tm_isdst;
};

static unsigned long mktime(const unsigned int year0, const unsigned int mon0,
        const unsigned int day, const unsigned int hour,
        const unsigned int min, const unsigned int sec)
{
    unsigned int mon = mon0, year = year0;

    /* 1..12 -> 11,12,1..10 */
    if (0 >= (int) (mon -= 2)) {
        mon += 12;    /* Puts Feb last since it has leap day */
        year -= 1;
    }

    return ((((unsigned long)
          (year/4 - year/100 + year/400 + 367*mon/12 + day) +
          year*365 - 719499
        )*24 + hour /* now have hours */
      )*60 + min /* now have minutes */
    )*60 + sec; /* finally seconds */
}


static struct tm * localtime_r(time_t *srctime,struct tm *tm_time)
{
    long int n32_Pass4year,n32_hpery;
    const static char Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const static int ONE_YEAR_HOURS = 8760;

    time_t time = *srctime;
    time=time+28800;
    tm_time->tm_isdst=0;
    if(time < 0)
    {
    time = 0;
    }

    tm_time->tm_sec=(int)(time % 60);
    time /= 60;
    tm_time->tm_min=(int)(time % 60);
    time /= 60;
    tm_time->tm_wday=(time/24+4)%7;
    n32_Pass4year=((unsigned int)time / (1461L * 24L));
    tm_time->tm_year=(n32_Pass4year << 2)+70;
    time %= 1461L * 24L;
    tm_time->tm_yday=(time/24)%365;

    for (;;)
    {
        n32_hpery = ONE_YEAR_HOURS;
        if ((tm_time->tm_year & 3) == 0)
        {
            n32_hpery += 24;
        }

        if (time < n32_hpery)
        {
            break;
        }

        tm_time->tm_year++;
        time -= n32_hpery;
    }

    tm_time->tm_hour=(int)(time % 24);
    time /= 24;
    time++;

    if ((tm_time->tm_year & 3) == 0)
    {
        if (time > 60)
        {
            time--;
        }
        else
        {
            if (time == 60)
            {
                tm_time->tm_mon = 1;
                tm_time->tm_mday = 29;
                return tm_time;
            }
        }
    }

    for (tm_time->tm_mon = 0;Days[tm_time->tm_mon] < time;tm_time->tm_mon++)
    {
        time -= Days[tm_time->tm_mon];
    }

    tm_time->tm_mday = (int)(time);
    return tm_time;
}

int main(void)
{
    uint32_t now = 1451577600; /*2016/01/01 00:00:00*/
    struct tm tm = {0};
    char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    localtime_r(&now, &tm);
    printf("%d/%d/%d \n", 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday);
    printf("%s %d:%d:%d \n", wday[tm.tm_wday], tm.tm_hour, tm.tm_min, tm.tm_sec);
    now = mktime(1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf("rtc value : %d\n", now);
    return 0;
}
