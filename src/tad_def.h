#ifndef __TAD_DEFINE_HEADER
#define __TAD_DEFINE_HEADER


/* enums */

/**
 * TadEvent:
 * @TAD_EVENT_PEN_IN:		pen is started to be recognized.
 * @TAD_EVENT_PEN_OUT:		pen is up.
 * @TAD_EVENT_TOUCH_IN:		touch is started.
 * @TAD_EVENT_TOUCH_OUT:		touch is ended.
 *
 * The stimulus of this application, for supporting transition of #TadState.
 */
typedef enum {
	TAD_EVENT_PEN_IN,
	TAD_EVENT_PEN_OUT,
	TAD_EVENT_TOUCH_IN,
	TAD_EVENT_TOUCH_OUT,
	TAD_EVENT_COUNT,
	TAD_EVENT_INVAILD = -1
} TadEvent;


/*
 * TadPrintType:
 * @TAD_PRINT_NO:		prints nothing - default behavior.
 * @TAD_PRINT_HUMAN:	human readable event printing.
 * @TAD_PRINT_MACHINE:  prints event and state in 4 char (+ '\n')
 *
 * Print type for event and status.
 */
typedef enum {
	TAD_PRINT_TYPE_NO,
	TAD_PRINT_TYPE_HUMAN,
	TAD_PRINT_TYPE_MACHINE
} TadPrintType;

extern const char* TadPrintTypeStr[];


/*
 * TadState:
 * @TAD_STATE_ZERO:			neutral state.
 * @TAD_STATE_PEN:			pen recognizing state.
 * @TAD_STATE_REST_PEN:		pen recognizing and a hand is resting.
 * @TAD_STATE_REST_STILL:   pen is up while hand is resting.
 * @TAD_STATE_TOUCHING:		user touching screen for touch input.
 * @TAD_STATE_COUNT:		number of TadState.
 *
 * State of this application, for delaying enabling of touchscreen till it is
 * clear, which requires some bit of complex.
 */
typedef enum {
	TAD_STATE_ZERO,
	TAD_STATE_PEN,
	TAD_STATE_REST_PEN,
	TAD_STATE_REST_STILL,
	TAD_STATE_TOUCHING,
	TAD_STATE_COUNT,

    TAD_STATE_INVALID = -1
} TadState;

/*
 * TadTransType:
 * @TAD_TRANS_ZERO:				plain and simple state transition.
 * @TAD_TRANS_CLEAR_TO_ENABLE:  touchscreen should be cleared before enabled.
 * @TAD_TRANS_COUNT:			number of TadTransType.
 */
typedef enum {
  TAD_TRANS_ZERO,
  TAD_TRANS_CLEAR_TO_ENABLE,
  TAD_TRANS_COUNT,

  TAD_TRANS_INVALID = -1
} TadTransType;

#endif  //TAD_DEFINE_HEADER
