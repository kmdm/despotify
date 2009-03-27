/*
 * $Id$
 */

#include <ruby.h>
#include <despotify.h>

#include "main.h"
#include "session.h"
#include "playlist.h"
#include "track.h"
#include "artist.h"
#include "album.h"

#define ALBUM_METHOD_HEADER \
	rb_despotify_album *album; \
	VALUE2ALBUM(self, album);


static VALUE
rb_despotify_album_alloc(VALUE klass) {
	rb_despotify_album *track;
	VALUE obj = Data_Make_Struct(klass, rb_despotify_album,
	                             NULL, free, track);

	return obj;
}

static VALUE
rb_despotify_album_new(VALUE self) {
	return self;
}

VALUE
rb_despotify_album_new_from_album(despotify_album *a) {
	VALUE obj;
	rb_despotify_album *album;

	if (!a)
		return Qnil;

	obj = rb_despotify_album_alloc(cAlbum);
	VALUE2ALBUM(obj, album);

	album->real = a;

	return obj;
}


static VALUE
rb_despotify_album_tracks(VALUE self) {
	ALBUM_METHOD_HEADER
	despotify_track *t = NULL;
	VALUE tracks;

	tracks = rb_ary_new();

	for (t = album->real->tracks; t; t = t->next) {
		rb_ary_push(tracks, rb_despotify_track_new_from_track(t));
	}

	return tracks;
}

static VALUE
rb_despotify_album_lookup(VALUE self, VALUE key) {
	ALBUM_METHOD_HEADER
	char *keyval = NULL;

	keyval = StringValuePtr(key);

	if (!strcmp(keyval, "name"))
		return rb_str_new2(album->real->name);
	else if (!strcmp(keyval, "id"))
		return rb_str_new2(album->real->id);
	else if (!strcmp(keyval, "num_tracks"))
		return INT2NUM(album->real->num_tracks);
	else if (!strcmp(keyval, "year"))
		return INT2NUM(album->real->year);
	else if (!strcmp(keyval, "cover_id"))
		return rb_str_new2(album->real->id);
	else if (!strcmp(keyval, "popularity"))
		return rb_float_new(album->real->popularity);

	return Qnil;
}


#define ALBUM_METADATA_ADD(hash, key) \
	keyval = rb_str_new2(key); \
	rb_hash_aset(hash, keyval, rb_despotify_album_lookup(self, keyval))

static VALUE
rb_despotify_album_metadata(VALUE self) {
	ALBUM_METHOD_HEADER
	VALUE metadata;
	VALUE keyval;

	metadata = rb_hash_new();
	ALBUM_METADATA_ADD(metadata, "name");
	ALBUM_METADATA_ADD(metadata, "id");
	ALBUM_METADATA_ADD(metadata, "num_tracks");
	ALBUM_METADATA_ADD(metadata, "year");
	ALBUM_METADATA_ADD(metadata, "cover_id");
	ALBUM_METADATA_ADD(metadata, "popularity");


	return metadata;
}

static VALUE
rb_despotify_album_name(VALUE self) {
	ALBUM_METHOD_HEADER

	return rb_str_new2(album->real->name);
}

static VALUE
rb_despotify_album_id(VALUE self) {
	ALBUM_METHOD_HEADER

	return rb_str_new2(album->real->id);
}


VALUE
Init_despotify_album(VALUE mDespotify) {
	VALUE c;

	/* Despotify::Track */
	c = rb_define_class_under(mDespotify, "Album", rb_cObject);

	/* Remove new function until we can request album by id */
	rb_undef_method (rb_singleton_class(c), "new");

	rb_define_method(c, "name", rb_despotify_album_name, 0);
	rb_define_method(c, "id", rb_despotify_album_id, 0);
	rb_define_method(c, "tracks", rb_despotify_album_tracks, 0);

	rb_define_method(c, "[]", rb_despotify_album_lookup, 1);
	rb_define_method(c, "metadata", rb_despotify_album_metadata, 0);

	return c;
}