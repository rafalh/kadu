/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SHARED_BASE_H
#define SHARED_BASE_H

#include <QtCore/QExplicitlySharedDataPointer>
#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "storage/storable-object.h"

/**
 * @addtogroup Storage
 * @{
 */

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Declares standard interface for SharedBase class. Use this macro in each SharedBase subclass declaration.
 * @param className name of defined class
 *
 * Declares operators for SharedBase subclass.
 */
#define KaduSharedBaseClass(className)\
public:\
	className & operator = (const className &copy);\
\
	bool operator == (const className &compare) const\
	{\
		return data() == compare.data();\
	}\
\
	bool operator != (const className &compare) const\
	{\
		return data() != compare.data();\
	}\
\
	int operator < (const className &compare) const\
	{\
		return data() < compare.data();\
	}\
private:

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines standard interface for SharedBase class. Use this macro in each SharedBase subclass definition file.
 * @param className name of defined class
 *
 * Defines operators for SharedBase subclass.
 */
#define KaduSharedBaseClassImpl(className)\
	className & className::operator = (const className &copy)\
	{\
		setData(copy.data());\
		return *this;\
	}\
	

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Declares getter for given property of SharedBase's Shared class.
 * @param type type of property
 * @param name name of getter
 * @param capitalized_name name of property
 *
 * Declares getter for a delegated property of @link Shared @endlink class stored in
 * @link SharedBase @endlink. Getter is named 'name'.
 */
#define KaduSharedBase_PropertyRead(type, name, capitalized_name) \
	type name() const;

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Declares setter for given property of StorableObject.
 * @param type type of property
 * @param name name of value parameter
 * @param capitalized_name name of property (setter will be called set##capitalized_name)
 *
 * Declares setter for a delegated property of @link Shared @endlink class stored in
 * @link SharedBase @endlink. Setter is named 'set##capitalized_name'.
 */
#define KaduSharedBase_PropertyWrite(type, name, capitalized_name) \
	void set##capitalized_name(type name) const;

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Declares getter and setter for given property of SharedBase's Shared class.
 * @param type type of property
 * @param name name of getter
 * @param capitalized_name name of property
 *
 * Declares getter and setter for a delegated property of @link Shared @endlink class stored in
 * @link SharedBase @endlink. Getter is named 'name'. Setter is named 'set##capitalized_name'.
 */
#define KaduSharedBase_Property(type, name, capitalized_name) \
	KaduSharedBase_PropertyRead(type, name, capitalized_name) \
	KaduSharedBase_PropertyWrite(type, name, capitalized_name)

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Declares getter for given boolean property of SharedBase's Shared class.
 * @param capitalized_name name of property
 *
 * Declares getter for a boolean delegated property of @link Shared @endlink class stored in
 * @link SharedBase @endlink. Getter is named 'is##capitalized_name'.
 */
#define KaduSharedBase_PropertyBoolRead(capitalized_name) \
	bool is##capitalized_name() const;

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Declares setter for given boolean property of StorableObject.
 * @param capitalized_name name of property (setter will be called set##capitalized_name)
 *
 * Declares setter for a delegated boolean property of @link Shared @endlink class stored in
 * @link SharedBase @endlink. Setter is named 'set##capitalized_name'.
 */
#define KaduSharedBase_PropertyBoolWrite(capitalized_name) \
	void set##capitalized_name(bool name) const;

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Declares getter and setter for given boolean property of SharedBase's Shared class.
 * @param capitalized_name name of property
 *
 * Declares getter for a boolean delegated property of @link Shared @endlink class stored in
 * @link SharedBase @endlink. Getter is named 'is##capitalized_name'. Setter is named 'set##capitalized_name'.
 */
#define KaduSharedBase_PropertyBool(capitalized_name) \
	KaduSharedBase_PropertyBoolRead(capitalized_name) \
	KaduSharedBase_PropertyBoolWrite(capitalized_name)

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines getter for given property of SharedBase's Shared class.
 * @param type type of property
 * @param name name of getter
 * @param capitalized_name name of property
 * @param default default value
 *
 * If @link SharedBase @endlink does not store any @link Shared @endlink the default value
 * is returned. Else, value of Shared method 'name' is returned.
 */
#define KaduSharedBase_PropertyReadDef(class_name, type, name, capitalized_name, default) \
	type class_name::name() const\
	{\
		return isNull()\
			? default\
			: data()->name();\
	}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines setter for given property of SharedBase's Shared class.
 * @param type type of property
 * @param name name of getter
 * @param capitalized_name name of property
 *
 * If @link SharedBase @endlink does not store any @link Shared @endlink nothing is done.
 * Else, method 'set##capitalized_name' of Shared object is called, so the property value
 * is set.
 */
#define KaduSharedBase_PropertyWriteDef(class_name, type, name, capitalized_name) \
	void class_name::set##capitalized_name(type name) const\
	{\
		if (!isNull())\
			data()->set##capitalized_name(name);\
	}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines getter and setter for given property of SharedBase's Shared class.
 * @param type type of property
 * @param name name of getter
 * @param capitalized_name name of property
 * @param default default value
 *
 * Defines getter (@link KaduSharedBase_PropertyReadDef @endlink) and setter
 * (@link KaduSharedBase_PropertyWriteDef @endlink) got given delegated property.
 */
#define KaduSharedBase_PropertyDef(class_name, type, name, capitalized_name, default) \
	KaduSharedBase_PropertyReadDef(class_name, type, name, capitalized_name, default) \
	KaduSharedBase_PropertyWriteDef(class_name, type, name, capitalized_name)

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines getter for given boolean property of SharedBase's Shared class.
 * @param name name of getter
 * @param capitalized_name name of property
 * @param default default value
 *
 * If @link SharedBase @endlink does not store any @link Shared @endlink the default value
 * is returned. Else, value of Shared method 'is##capitalized_name' is returned.
 */
#define KaduSharedBase_PropertyBoolReadDef(class_name, capitalized_name, default) \
	bool class_name::is##capitalized_name() const\
	{\
		return isNull()\
			? default\
			: data()->is##capitalized_name();\
	}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines setter for given boolean property of SharedBase's Shared class.
 * @param name name of getter
 * @param capitalized_name name of property
 *
 * If @link SharedBase @endlink does not store any @link Shared @endlink nothing is done.
 * Else, method 'set##capitalized_name' of Shared object is called, so the property value
 * is set.
 */
#define KaduSharedBase_PropertyBoolWriteDef(class_name, capitalized_name) \
	void class_name::set##capitalized_name(bool value) const\
	{\
		if (!isNull())\
			data()->set##capitalized_name(value);\
	}

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Defines getter and setter for given boolean property of SharedBase's Shared class.
 * @param name name of getter
 * @param capitalized_name name of property
 * @param default default value
 *
 * Defines getter (@link KaduSharedBase_PropertyBoolReadDef @endlink) and setter
 * (@link KaduSharedBase_PropertyBoolWriteDef @endlink) got given delegated property.
 */
#define KaduSharedBase_PropertyBoolDef(class_name, capitalized_name, default) \
	KaduSharedBase_PropertyBoolReadDef(class_name, capitalized_name, default) \
	KaduSharedBase_PropertyBoolWriteDef(class_name, capitalized_name)

/**
 * @class SharedBase
 * @author Rafal 'Vogel' Malinowski
 * @short Class that can contain instance of Shared class that holds this class' data.
 * @param T class derivered from Shared type, it holds data for this SharedBase
 *
 * This class is contructed to allow copying objects without copying their data. Each object
 * of SharedBase class contains only pointer (reference-counted) to Shared class that
 * contains real data. So every copied object will have the same data, changes in one object
 * will be visible in all copied object. This is behaviour similar to references and very
 * similar to Java objects.
 *
 * Empty objects are called 'null' objects and can be tested by isNull method.
 *
 * If no more objects hold given data object, it will be destroyed and memory will be fred.
 *
 * If T class is QObject then SharedBase inherits all of its signals and slots, so this
 * is possible:
 *
 * <pre>
 * ContactShared *contactData = ...
 * SharedBase&lt;Contact&gt; contact(contactData);
 * connect(contactData, SIGNAL(...), this, SLOT(...));
 * connect(contact, SIGNAL(...), this, SLOT(...));
 * </pre>
 *
 * Both connect will connect to exactly the same signal. This is thanks to QObject-cast operator.
 */
template<class T>
class SharedBase
{
	QExplicitlySharedDataPointer<T> Data;

protected:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short sets new data for object
	 * @param data new data for object
	 *
	 * Sets new data for this object. All reference counters are updated properly.
	 */
	void setData(T *data)
	{
		Data = data;
	}

public:
	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Contructs empty (null) object.
	 *
	 * Contructs empty (null) object. It contains no data.
	 */
	SharedBase() :
			Data(0)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Contructs object with given data.
	 * @param data data this object will hold
	 *
	 * Contructs object with given data. Reference counters of data are updated.
	 */
	explicit SharedBase(T *data) :
			Data(data)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Copies object.
	 * @param copy object that will be copied
	 *
	 * Copies copy object. Source and resulting object will have the same data. Udpate
	 * in one will result in update with second. Only way to break this dependence
	 * is to use assignment operator.
	 */
	SharedBase(const SharedBase &copy) :
			Data(copy.Data)
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Destroys object.
	 *
	 * Destroys object. Reference counters of data object will be updated. If this object was
	 * the last one storing given data, the data object will be destroyed as well.
	 */
	virtual ~SharedBase()
	{
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Cast object to QObject * variable.
	 * @return this object as QObject * variable
	 *
	 * Returns data object. Data object must be class derivered from QObject.
	 * This method allows for signal/slot connecting SharedBase objects.
	 */
	operator const QObject * () const
	{
		return Data.data();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Cast object to T * variable.
	 * @return this object as T * variable
	 *
	 * Returns data object. Allows for use SharedBase as T * variables.
	 */
	operator T * () const
	{
		return Data.data();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Cast object to boolean variable.
	 * @return true if object is not null, false, if object is null
	 *
	 * Returns true if object is not null (contains data) and false, if object is null
	 * (does not contains an data).
	 */
	operator bool () const // for ! and ifs
	{
		return Data.data();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Returns stored data object.
	 * @return stored data object
	 *
	 * Returns data object.
	 */
	T * data() const
	{
		return Data.data();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Check if object contains any data.
	 * @return true if object is not null, false, if object is null
	 *
	 * Returns true if object is not null (contains data) and false, if object is null
	 * (does not contains an data).
	 */
	bool isNull() const
	{
		return !Data.data();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Check if two objects are not equal.
	 * @param compare object to compare with
	 * @return true, if two objects are different
	 *
	 * Returns true if two objects contains different data objects (or one of them is null).
	 */
	bool operator != (const SharedBase<T> &compare) const
	{
		return Data != compare.Data;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Check if one objects is smaller than second.
	 * @param compare object to compare with
	 * @return true, if this object is smaller than compare
	 *
	 * Objects are compared by pointers to their data objects. Object with smaller pointer value
	 * is considered as smaller. Methos is used mainly in QMap.
	 */
	int operator < (const SharedBase<T> &compare) const
	{
		return Data < compare.Data;
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Delegates blockUpdatedSignal method to Shared object.
	 *
	 * @see Shared::blockUpdatedSignal
	 */
	void blockUpdatedSignal()
	{
		if (!isNull())
			Data->blockUpdatedSignal();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Delegates unblockUpdatedSignal method to Shared object.
	 *
	 * @see Shared::unblockUpdatedSignal
	 */
	void unblockUpdatedSignal()
	{
		if (!isNull())
			Data->unblockUpdatedSignal();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Delegates store method to Shared object.
	 *
	 * @see Shared::store
	 */
	void store()
	{
		if (!isNull())
			Data->store();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Delegates ensureStored method to Shared object.
	 *
	 * @see Shared::ensureStored
	 */
	void ensureStored()
	{
		if (!isNull())
			Data->ensureStored();
	}

	/**
	 * @author Rafal 'Vogel' Malinowski
	 * @short Removes object completly.
	 *
	 * If object is null, this method does nothing.
	 * This method calls @link<Shared::aboutToBeRemoved @endlink. Then data object is removed from
	 * storage (it must be StorableObject then).
	 */
	void remove()
	{
		if (!isNull())
		{
			Data->aboutToBeRemoved();
			Data->removeFromStorage();
		}
	}

	KaduSharedBase_Property(QUuid, uuid, Uuid)

};

template<class T>
KaduSharedBase_PropertyReadDef(SharedBase<T>, QUuid, uuid, Uuid, QUuid());
template<class T>
KaduSharedBase_PropertyWriteDef(SharedBase<T>, QUuid, uuid, Uuid);

/**
 * @author Rafal 'Vogel' Malinowski
 * @short Computes hash for given SharedBase object.
 * @return hash for given SharedBase object
 *
 * Hash is computed by uuid field of data object. All null objects have the same hash.
 * Used for QHash objects.
 */
template<class T>
uint qHash(const SharedBase<T> &sharedBase)
{
	return qHash(sharedBase.uuid().toString());
}

/**
 * @}
 */

#endif // SHARED_BASE_H
