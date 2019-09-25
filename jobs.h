#ifndef JOBS_H_INCLUDED
#define ECHO_H_INCLUDED

void addJob(pid_t pid, char *command);
void deleteJob(pid_t pid);

void jobs();
void kjob();
void overkill();


#endif