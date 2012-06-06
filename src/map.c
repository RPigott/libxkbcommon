/**
 * Copyright © 2012 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Author: Daniel Stone <daniel@fooishbar.org>
 */

/************************************************************
 * Copyright (c) 1993 by Silicon Graphics Computer Systems, Inc.
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation, and that the name of Silicon Graphics not be
 * used in advertising or publicity pertaining to distribution
 * of the software without specific prior written permission.
 * Silicon Graphics makes no representation about the suitability
 * of this software for any purpose. It is provided "as is"
 * without any express or implied warranty.
 *
 * SILICON GRAPHICS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SILICON
 * GRAPHICS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * ********************************************************/

#include "xkb-priv.h"

/**
 * Returns the total number of modifiers active in the keymap.
 */
_X_EXPORT xkb_mod_index_t
xkb_map_num_mods(struct xkb_keymap *keymap)
{
    xkb_mod_index_t i;

    for (i = 0; i < XkbNumVirtualMods; i++)
        if (!keymap->names->vmods[i])
            break;

    /* We always have all the core modifiers (for now), plus any virtual
     * modifiers we may have defined, and then shift to one-based indexing. */
    return i + Mod5MapIndex + 1;
}

/**
 * Return the name for a given modifier.
 */
_X_EXPORT const char *
xkb_map_mod_get_name(struct xkb_keymap *keymap, xkb_mod_index_t idx)
{
    if (idx >= xkb_map_num_mods(keymap))
        return NULL;

    /* First try to find a legacy modifier name. */
    switch (idx) {
    case ShiftMapIndex:
        return "Shift";
    case ControlMapIndex:
        return "Control";
    case LockMapIndex:
        return "Caps Lock";
    case Mod1MapIndex:
        return "Mod1";
    case Mod2MapIndex:
        return "Mod2";
    case Mod3MapIndex:
        return "Mod3";
    case Mod4MapIndex:
        return "Mod4";
    case Mod5MapIndex:
        return "Mod5";
    default:
        break;
    }

    /* If that fails, try to find a virtual mod name. */
    return keymap->names->vmods[idx - Mod5MapIndex];
}

/**
 * Returns the index for a named modifier.
 */
_X_EXPORT xkb_mod_index_t
xkb_map_mod_get_index(struct xkb_keymap *keymap, const char *name)
{
    xkb_mod_index_t i;

    if (strcasecmp(name, "Shift") == 0)
        return ShiftMapIndex;
    if (strcasecmp(name, "Control") == 0)
        return ControlMapIndex;
    if (strcasecmp(name, "Caps Lock") == 0)
        return LockMapIndex;
    if (strcasecmp(name, "Mod1") == 0)
        return Mod1MapIndex;
    if (strcasecmp(name, "Mod2") == 0)
        return Mod2MapIndex;
    if (strcasecmp(name, "Mod3") == 0)
        return Mod3MapIndex;
    if (strcasecmp(name, "Mod4") == 0)
        return Mod4MapIndex;
    if (strcasecmp(name, "Mod5") == 0)
        return Mod5MapIndex;

    for (i = 0; i < XkbNumVirtualMods && keymap->names->vmods[i]; i++) {
        if (strcasecmp(name, keymap->names->vmods[i]) == 0)
            return i + Mod5MapIndex;
    }

    return XKB_GROUP_INVALID;
}

/**
 * Return the total number of active groups in the keymap.
 */
_X_EXPORT xkb_group_index_t
xkb_map_num_groups(struct xkb_keymap *keymap)
{
    xkb_group_index_t ret = 0;
    xkb_group_index_t i;

    for (i = 0; i < XkbNumKbdGroups; i++)
        if (keymap->compat->groups[i].mask)
            ret++;

    return ret;
}

/**
 * Returns the name for a given group.
 */
_X_EXPORT const char *
xkb_map_group_get_name(struct xkb_keymap *keymap, xkb_group_index_t idx)
{
    if (idx >= xkb_map_num_groups(keymap))
        return NULL;

    return keymap->names->groups[idx];
}

/**
 * Returns the index for a named group.
 */
_X_EXPORT xkb_group_index_t
xkb_map_group_get_index(struct xkb_keymap *keymap, const char *name)
{
    xkb_group_index_t num_groups = xkb_map_num_groups(keymap);
    xkb_group_index_t i;

    for (i = 0; i < num_groups; i++) {
        if (strcasecmp(keymap->names->groups[i], name) == 0)
            return i;
    }

    return XKB_GROUP_INVALID;
}

/**
 * Returns the number of groups active for a particular key.
 */
_X_EXPORT xkb_group_index_t
xkb_key_num_groups(struct xkb_keymap *keymap, xkb_keycode_t key)
{
    return XkbKeyNumGroups(keymap, key);
}

/**
 * Return the total number of active LEDs in the keymap.
 */
_X_EXPORT xkb_led_index_t
xkb_map_num_leds(struct xkb_keymap *keymap)
{
    xkb_led_index_t ret = 0;
    xkb_led_index_t i;

    for (i = 0; i < XkbNumIndicators; i++)
        if (keymap->indicators->maps[i].which_groups ||
            keymap->indicators->maps[i].which_mods ||
            keymap->indicators->maps[i].ctrls)
            ret++;

    return ret;
}

/**
 * Returns the name for a given group.
 */
_X_EXPORT const char *
xkb_map_led_get_name(struct xkb_keymap *keymap, xkb_led_index_t idx)
{
    if (idx >= xkb_map_num_leds(keymap))
        return NULL;

    return keymap->names->indicators[idx];
}

/**
 * Returns the index for a named group.
 */
_X_EXPORT xkb_group_index_t
xkb_map_led_get_index(struct xkb_keymap *keymap, const char *name)
{
    xkb_led_index_t num_leds = xkb_map_num_leds(keymap);
    xkb_led_index_t i;

    for (i = 0; i < num_leds; i++) {
        if (strcasecmp(keymap->names->indicators[i], name) == 0)
            return i;
    }

    return XKB_LED_INVALID;
}

/**
 * Returns the level to use for the given key and state, or -1 if invalid.
 */
_X_EXPORT unsigned int
xkb_key_get_level(struct xkb_state *state, xkb_keycode_t key,
                  unsigned int group)
{
    struct xkb_keymap *keymap = xkb_state_get_map(state);
    struct xkb_key_type *type = XkbKeyType(keymap, key, group);
    struct xkb_kt_map_entry *entry;
    unsigned int active_mods;

    active_mods = xkb_state_serialize_mods(state, XKB_STATE_EFFECTIVE);
    active_mods &= type->mods.mask;

    darray_foreach(entry, type->map) {
        if (!entry->active)
            continue;

        if (entry->mods.mask == active_mods)
            return entry->level;
    }

    return 0;
}

/**
 * Returns the group to use for the given key and state, taking
 * wrapping/clamping/etc into account.
 */
_X_EXPORT unsigned int
xkb_key_get_group(struct xkb_state *state, xkb_keycode_t key)
{
    struct xkb_keymap *keymap = xkb_state_get_map(state);
    unsigned int info = XkbKeyGroupInfo(keymap, key);
    unsigned int num_groups = XkbKeyNumGroups(keymap, key);
    unsigned int ret = xkb_state_serialize_group(state, XKB_STATE_EFFECTIVE);

    if (ret < XkbKeyNumGroups(keymap, key))
        return ret;

    switch (XkbOutOfRangeGroupAction(info)) {
    case XkbRedirectIntoRange:
        ret = XkbOutOfRangeGroupInfo(info);
        if (ret >= num_groups)
            ret = 0;
        break;
    case XkbClampIntoRange:
        ret = num_groups - 1;
        break;
    case XkbWrapIntoRange:
    default:
        ret %= num_groups;
        break;
    }

    return ret;
}

/**
 * As below, but takes an explicit group/level rather than state.
 */
int
xkb_key_get_syms_by_level(struct xkb_keymap *keymap, xkb_keycode_t key,
                          unsigned int group, unsigned int level,
                          const xkb_keysym_t **syms_out)
{
    int num_syms;

    if (group >= XkbKeyNumGroups(keymap, key))
        goto err;
    if (level >= XkbKeyGroupWidth(keymap, key, group))
        goto err;

    num_syms = XkbKeyNumSyms(keymap, key, group, level);
    if (num_syms == 0)
        goto err;

    *syms_out = XkbKeySymEntry(keymap, key, group, level);
    return num_syms;

err:
    *syms_out = NULL;
    return 0;
}

/**
 * Provides the symbols to use for the given key and state.  Returns the
 * number of symbols pointed to in syms_out.
 */
_X_EXPORT int
xkb_key_get_syms(struct xkb_state *state, xkb_keycode_t key,
                 const xkb_keysym_t **syms_out)
{
    struct xkb_keymap *keymap = xkb_state_get_map(state);
    int group;
    int level;

    if (!state || key < keymap->min_key_code || key > keymap->max_key_code)
        return -1;

    group = xkb_key_get_group(state, key);
    if (group == -1)
        goto err;
    level = xkb_key_get_level(state, key, group);
    if (level == -1)
        goto err;

    return xkb_key_get_syms_by_level(keymap, key, group, level, syms_out);

err:
    *syms_out = NULL;
    return 0;
}
