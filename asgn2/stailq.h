/*
 * Adapted from sys/queue.h for the lwp data structures
 */

/*
 * This macro picks out which element of the thread structure
 * we're threading on our lists e.g.
 *
 * #define STAILQ_NEXT(t)			((t)->fieldname)
 */
#if !defined(STAILQ_NEXT)
#error "must have an STAILQ_NEXT defined"
#endif

#define STAILQ_HEAD(name)	\
	struct name {		\
		thread front;	\
		thread *lastp;	\
	}

#define STAILQ_INITIALIZER(h)		{ NULL, &(h).front }
#define STAILQ_FIRST(hp)		((hp)->front)

#define STAILQ_INSERT_TAIL(hp, t) do {			\
	STAILQ_NEXT(t) = NULL;				\
	*((hp)->lastp) = t;				\
	(hp)->lastp = &STAILQ_NEXT(t);			\
} while (0)

#define STAILQ_REMOVE_HEAD(hp) do {			\
	const thread f = (STAILQ_FIRST(hp) =		\
		STAILQ_NEXT(STAILQ_FIRST(hp)));		\
	if (f == NULL) {				\
		(hp)->lastp = &STAILQ_FIRST(hp);	\
	}						\
} while (0)

#define STAILQ_REMOVE(hp, v) do {			\
	if (STAILQ_FIRST(hp) == (v)) {			\
		STAILQ_REMOVE_HEAD(hp);			\
	} else {					\
		thread t = STAILQ_FIRST(hp);		\
		while (STAILQ_NEXT(t) != (v)) {		\
			t = STAILQ_NEXT(t);		\
		}					\
		const thread n = (STAILQ_NEXT(t) =	\
			STAILQ_NEXT(STAILQ_NEXT(t)));	\
		if (n == NULL) {			\
			(hp)->lastp = &STAILQ_NEXT(t);	\
		}					\
	}						\
} while (0)

#define STAILQ_FOREACH(decl, t, hp)			\
	for (decl t = STAILQ_FIRST(hp); t; t = STAILQ_NEXT(t))
