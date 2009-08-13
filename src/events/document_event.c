/*
 * This file is part of libdom.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 Bo Yang <struggleyb.nku@gmail.com>
 */

#include <assert.h>

#include <libwapcaplet/libwapcaplet.h>

#include "core/string.h"
#include "core/node.h"
#include "core/document.h"
#include "events/document_event.h"
#include "events/event.h"
#include "events/ui_event.h"
#include "events/custom_event.h"
#include "events/text_event.h"
#include "events/keyboard_event.h"
#include "events/mouse_event.h"
#include "events/mouse_multi_wheel_event.h"
#include "events/mouse_wheel_event.h"
#include "events/mutation_event.h"
#include "events/mutation_name_event.h"

#include "utils/utils.h"

static const char *__event_types[] = {
	"Event",
	"CustomEvent",
	"UIEvent",
	"TextEvent",
	"KeyboardEvent",
	"MouseEvent",
	"MouseMultiWheelEvent",
	"MouseWheelEvent",
	"MutationEvent",
	"MutationNameEvent"
};

/**
 * Initialise this DocumentEvent
 *
 * \param doc      The document object
 * \param dei      The DocumentEvent internal object
 * \param actions  The default action fetcher, the browser should provide such
 *                 a function to Event module.
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_document_event_internal_initialise(struct dom_document *doc,
		dom_document_event_internal *dei, 
		dom_events_default_action_fetcher actions)
{
	lwc_error err;
	lwc_context *ctx = _dom_document_get_intern_context(doc);
	int i = 0;

	for (; i < DOM_EVENT_COUNT; i++) {
		err = lwc_context_intern(ctx, __event_types[i],
				SLEN(__event_types[i]), &dei->event_types[i]);
		if (err != lwc_error_ok)
			return _dom_exception_from_lwc_error(err);
	}

	dei->actions = actions;

	return DOM_NO_ERR;
}

/* Finalise this DocumentEvent */
void _dom_document_event_internal_finalise(struct dom_document *doc,
		dom_document_event_internal *dei)
{
	/* Nothing to do here */
	UNUSED(doc);
	UNUSED(dei);
	return;
}

/*-------------------------------------------------------------------------*/
/* Public API */

/**
 * Create an Event object 
 *
 * \param de    The DocumentEvent object
 * \param type  The Event type
 * \param evt   The returned Event object
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 */
dom_exception _dom_document_event_create_event(dom_document_event *de,
		struct dom_string *type, struct dom_event **evt)
{
	lwc_context *ctx = NULL;
	lwc_string *t = NULL;
	dom_exception err;
	struct dom_document *doc = de;

	dom_string_get_intern(type, &ctx, &t);
	if (t == NULL) {
		ctx = _dom_document_get_intern_context(doc);
		err = _dom_string_intern(type, ctx, &t);
		if (err != DOM_NO_ERR)
			return err;
	}

	assert(t != NULL);
	
	int i, et = 0;
	dom_document_event_internal *dei = &de->dei;
	for (i = 0; i < DOM_EVENT_COUNT; i++) {
		if (dei->event_types[i] == t) {
			et = i;
			break;
		}
	}
	lwc_context_string_unref(ctx, t);

	switch (et) {
		case DOM_EVENT:
			err = _dom_event_create(doc, evt);
			break;
		case DOM_CUSTOM_EVENT:
			err = _dom_custom_event_create(doc,
					(dom_custom_event **) evt);
			break;
		case DOM_UI_EVENT:
			err = _dom_ui_event_create(doc, (dom_ui_event **) evt);
			break;
		case DOM_TEXT_EVENT:
			err = _dom_text_event_create(doc,
					(dom_text_event **) evt);
			break;
		case DOM_KEYBOARD_EVENT:
			err = _dom_keyboard_event_create(doc,
					(dom_keyboard_event **) evt);
			break;
		case DOM_MOUSE_EVENT:
			err = _dom_mouse_event_create(doc,
					(dom_mouse_event **) evt);
			break;
		case DOM_MOUSE_MULTI_WHEEL_EVENT:
			err = _dom_mouse_multi_wheel_event_create(doc, 
					(dom_mouse_multi_wheel_event **) evt);
			break;
		case DOM_MOUSE_WHEEL_EVENT:
			err = _dom_mouse_wheel_event_create(doc,
					(dom_mouse_wheel_event **) evt);
			break;
		case DOM_MUTATION_EVENT:
			err = _dom_mutation_event_create(doc,
					(dom_mutation_event **) evt);
			break;
		case DOM_MUTATION_NAME_EVENT:
			err = _dom_mutation_name_event_create(doc, 
					(dom_mutation_name_event **) evt);
			break;
		default:
			assert("Should never be here" == NULL);
	}

	return err;
}

/**
 * Tests if the implementation can generate events of a specified type
 *
 * \param de         The DocumentEvent
 * \param namespace  The namespace of the event
 * \param type       The type of the event
 * \param can        True if we can generate such an event, false otherwise
 * \return DOM_NO_ERR on success, appropriate dom_exception on failure.
 *
 * We don't support this API now, so the return value should always
 * DOM_NO_SUPPORTED_ERR.
 */
dom_exception _dom_document_event_can_dispatch(dom_document_event *de,
		struct dom_string *namespace, struct dom_string *type,
		bool *can)
{
	UNUSED(de);
	UNUSED(namespace);
	UNUSED(type);
	UNUSED(can);

	return DOM_NOT_SUPPORTED_ERR;
}