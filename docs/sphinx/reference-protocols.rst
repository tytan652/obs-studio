Protocol API Reference (obs_protocol_t)
=======================================

Protocol allow the ability to define a protocol.
The `libobs/obs-protocol.h`_ file is the dedicated header
for implementing protocols

.. type:: obs_protocol_t

   A reference-counted protocol object.

.. type:: obs_weak_protocol_t

   A weak reference to an protocol object.

.. code:: cpp

   #include <obs.h>


Protocol Definition Structure (obs_protocol_info)
-------------------------------------------------

.. type:: struct obs_protocol_info

   Protocol definition structure.

.. member:: const char *obs_protocol_info.id

   Unique string identifier for the protocol (required).

.. member:: const char *(*obs_protocol_info.get_name)(void *type_data)

   Get the translated name of the protocol type.

   :param  type_data:  The type_data variable of this structure
   :return:            The translated name of the protocol type

.. member:: void *(*obs_protocol_info.create)(obs_data_t *settings, obs_protocol_t *protocol)

   Creates the implementation data for the protocol.

   :param  settings: Settings to initialize the protocol with
   :param  protocol:   Protocol that this data is associated with
   :return:          The implementation data associated with this protocol

.. member:: void (*obs_protocol_info.destroy)(void *data)

   Destroys the implementation data for the protocol.

.. member:: void *obs_protocol_info.type_data
            void (*obs_protocol_info.free_type_data)(void *type_data)

   Private data associated with this entry.  Note that this is not the
   same as the implementation data; this is used to differentiate
   between two different types if the same callbacks are used for more
   than one different type.

   (Optional)

General Protocol Functions
--------------------------

.. function:: void obs_register_protocol(struct obs_protocol_info *info)

   Registers an protocol type.  Typically used in
   :c:func:`obs_module_load()` or in the program's initialization phase.

---------------------

.. function:: const char *obs_protocol_get_display_name(const char *id)

   Calls the :c:member:`obs_protocol_info.get_name` callback to get the
   translated display name of an protocol type.

   :param    id:            The protocol type string identifier
   :return:                 The translated display name of an protocol type

---------------------

.. function:: obs_protocol_t *obs_protocol_create(const char *id, const char *name)

   Creates an protocol with the specified settings.

   Use obs_protocol_release to release it.

   :param   id:             The protocol type string identifier
   :param   name:           The desired name of the protocol.  If this is
                            not unique, it will be made to be unique
   :return:                 A reference to the newly created protocol, or
                            *NULL* if failed

---------------------

.. function:: void obs_protocol_addref(obs_protocol_t *protocol)
              void obs_protocol_release(obs_protocol_t *protocol)

   Adds/releases a reference to an protocol.  When the last reference is
   released, the protocol is destroyed.

---------------------

.. function:: obs_weak_protocol_t *obs_protocol_get_weak_procotol(obs_protocol_t *protocol)
              obs_protocol_t *obs_weak_protocol_get_protocol(obs_weak_protocol_t *weak)

   These functions are used to get a weak reference from a strong protocol
   reference, or a strong protocol reference from a weak reference.  If
   the protocol is destroyed, *obs_weak_protocol_get_protocol* will return
   *NULL*.

---------------------

.. function:: void obs_weak_protocol_addref(obs_weak_protocol_t *weak)
              void obs_weak_protocol_release(obs_weak_protocol_t *weak)

   Adds/releases a weak reference to an protocol.

---------------------

.. function:: const char *obs_protocol_get_name(const obs_protocol_t *protocol)

   :return: The name of the protocol

.. ---------------------------------------------------------------------------

.. _libobs/obs-protocol.h: https://github.com/obsproject/obs-studio/blob/master/libobs/obs-protocol.h
