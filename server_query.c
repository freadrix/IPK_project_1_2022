#include <stdio.h>
#include <unistd.h>


void getProcessorUsage(char proc_usage_str[]) {
    double a[4], b[4], avg_usage;
    FILE *fp;

    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %lf %lf %lf %lf",&a[0], &a[1], &a[2], &a[3]);
    fclose(fp);
    sleep(1);

    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%*s %lf %lf %lf %lf",&b[0], &b[1], &b[2], &b[3]);
    fclose(fp);

    avg_usage = ((b[0] + b[1] + b[2]) - (a[0] + a[1] + a[2])) /
                ((b[0] + b[1] + b[2] + b[3]) - (a[0] + a[1] + a[2] + a[3]));

    avg_usage = avg_usage * 100;
    snprintf(proc_usage_str, 10, "%.1lf%%", avg_usage);
}

void getHostname(char hostname_str[]) {
    FILE *fp;

    fp = fopen("/proc/sys/kernel/hostname", "r");
    fscanf(fp, "%s", hostname_str);
    fclose(fp);
}

void getProcessorName(char proc_name[]) {
    FILE *fp;

    fp = popen("cat /proc/cpuinfo | grep \"model name\" | head -n 1 "
               "| awk '{for(i=4;i<=NF-1;i++) printf $i\" \"} {printf $NF}'", "r");
    fgets(proc_name, 100, fp);
    pclose(fp);
}