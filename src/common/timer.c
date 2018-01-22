/*
*   Timer
*/

#include <common/timer.h>

list_t timer_list;

static uint32_t getInterval(timer_t* timer) {
    return timer->interval;
}

static uint32_t getNbrTicks(timer_t* timer) {
    return timer->nbr_ticks;
}

static uint32_t getRemainingTicks(timer_t* timer) {
    return timer->remaining_ticks;
}

static void setRemainingTicks(timer_t* timer, size_t remain) {
    timer->remaining_ticks = remain;
}

bool_t timer_tick(timer_t* timer) {

    timer->remaining_ticks--;

    if(timer->remaining_ticks <= 0) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void tick() {

    timer_t* it = (timer_t*) list_getFirst(&timer_list);

    if(it != NULL && timer_tick(it)) {

        // Control if an event occur at the same time
        while(it != NULL &&  getRemainingTicks(it) == 0) {

            eventqueue_push(it->queue, it->event);

            if(it->periodic) {
                timer_start(it);
            }

            list_deleteFirst(&timer_list);
            it = (timer_t*) list_getFirst(&timer_list);
        }
    }
}

static void addTimeout(timer_t* timer) {

    uint32_t ticks_sum = 0;

    for(timer_t* it  = (timer_t*)list_begin(&timer_list);
                 it != (timer_t*)list_end(&timer_list);
                 it  = (timer_t*)list_next(&timer_list))
    {
        ticks_sum += getRemainingTicks(it);

        // If the number of tick until this element is bigger that relTick
        // of the new element, it mean that the new element must be insterted
        // just before
        if(getNbrTicks(timer) < ticks_sum) {

            // Insert before the element, iterator doesn't change
            size_t remaining_ticks;
            remaining_ticks  = getNbrTicks(timer);
            remaining_ticks -= ticks_sum;
            remaining_ticks += getRemainingTicks(it);

            setRemainingTicks(timer, remaining_ticks);

            list_insert(&timer_list, 0, (uint8_t*)timer, FALSE, FALSE);

            // Correct the remain tick, only the next element
            setRemainingTicks(it, getRemainingTicks(it) - remaining_ticks);

            return;
        }
        // The timeout must occur at the same time
        else if(getNbrTicks(timer) == ticks_sum) {

             setRemainingTicks(timer, 0);

             it = (timer_t*)list_next(&timer_list);

             // Insert at the end of the other timeout that occur at the same time
             while(it != (timer_t*)list_end(&timer_list)) {

                if(getRemainingTicks(it) != 0) {
                  list_insert(&timer_list, 0, (uint8_t*)timer, FALSE, FALSE);

                  return;
                }

                it = (timer_t*)list_next(&timer_list);
             }

             list_add(&timer_list, 0, (uint8_t*)timer, FALSE);

             return;
        }
    }

    // If no timeout is already saved or the timeout must occur after all the others
    setRemainingTicks(timer, getNbrTicks(timer) - ticks_sum);
    list_add(&timer_list, 0, (uint8_t*)timer, FALSE);

}

void timer_init(timer_t* timer, eventqueue_t* queue, event_t* event,
                size_t interval, bool_t periodic, size_t id)
{
    timer->id        = id;
    timer->periodic  = periodic;
    timer->interval  = interval;
    timer->nbr_ticks = interval / SCHEDULER_TIME_UNIT;
    timer->queue     = queue;
    timer->event     = event;
}

void timer_start(timer_t* timer)
{
    addTimeout(timer);
}

void timer_stop(timer_t* timer)
{
    for(timer_t* it  = (timer_t*)list_begin(&timer_list);
                 it != (timer_t*)list_end(&timer_list);
                 it  = (timer_t*)list_next(&timer_list))
     {
         bool_t increm = TRUE;
         if(it->id == timer->id && it->queue == timer->queue)
         {
             size_t remainingTickCorrection = getRemainingTicks(it);

             // Warning: the next element of the list isn't compared if the loop
             // increments the iterator
             list_delete(&timer_list);
             it = (timer_t*)list_next(&timer_list);

             // Correct the remaining number of ticks
             if(it != (timer_t*)list_end(&timer_list)) {
                 setRemainingTicks(it, getRemainingTicks(it) + remainingTickCorrection);
             }

             increm = FALSE;
         }
         if(increm) {
             it = (timer_t*)list_next(&timer_list);
         }
     }
}
