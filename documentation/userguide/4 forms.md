---
title : Forms
---

## What is HTTP form data?

Forms are sets of key/value pairs. In the world wide web there is a specific way to encode forms: 
[URL encoding](https://en.wikipedia.org/wiki/Percent-encoding) (also known as percent encoding).

Form data is most of the time transported in the message body. The form data is concated to a 
single string where the key/value pairs are seperated by an ampersand (&) character. Key and 
value are separated by an equal sign (=).

```
// name:  Jörg Müller
// email: jmueller@example.com

name=J%C3%B6rg%20M%C3%BCller&email=jmueller%40example.com
```

To indicate that the content of the HTTP message is form data the `Content-Type` header is set as
`application/x-www-form-urlencoded`.


## Building Forms

Forms are first class citizen in ILEvator. The procedures with the namespace (prefix) `iv_forms`
can be used to easily build forms which can be used by the ILEvator HTTP client.

```
dcl-ds formData likeds(iv_lvarpuchar_t);

formData = iv_form_of(
    'name' : 'Jörg Müller' : 
    'email' : 'jmueller@example.com'
);

iv_form_free(formData);
```

The data structure `iv_lvarpuchar_t` used for `formData` contains the value and the length. More or 
less it is a string with a dynamic length. But in contrast to the data type `VARCHAR` the memory is 
manually allocated and must also be freed with `iv_form_free`.

Note: For your convenience the procedures `iv_form_ofMap` and `iv_form_ofString` has been overloaded 
with `iv_form_of` so that the system decides depending on the passed parameters which procedure
will be executed.

## Dynamic Form Content

If you want to build up the content of the form dynamically or step by step you can build an empty
or initialized list with the procedure `iv_buildList` and add entries with `iv_addEntryToList`.

```
dcl-s list pointer;
dcl-ds formData likeds(iv_lvarpuchar_t);

list = iv_buildList();
iv_addEntryToList(list : 'name' : 'Jörg Müller');
iv_addEntryToList(list : 'email' : 'jmueller@example.com');

formData = iv_form_of(list);

iv_form_free(formData);
iv_freeList(list);
```

Note: Don't forget to free the memory of the list with calling `iv_freeList`.
