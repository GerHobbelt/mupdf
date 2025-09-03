
#pragma once

typedef struct SDL_sem {
	int dummy;
} SDL_sem;

typedef struct SDL_mutex {
	int dummy;
} SDL_mutex;

SDL_sem *SDL_CreateSemaphore(int state);

void SDL_SemWait(SDL_sem *s);
void SDL_SemPost(SDL_sem *s);

SDL_mutex *SDL_CreateMutex(void);

void SDL_LockMutex(SDL_mutex *m);
void SDL_UnlockMutex(SDL_mutex *m);

int SDL_GetCPUCount(void);

void SDL_CreateThread(int worker(void *data), const char *name, void *data);
