
//metadoc Sequence copyright Steve Dekorte 2002
//metadoc Sequence license BSD revised
/*metadoc Sequence description
A Sequence is a container for a list of data elements. 
Immutable Sequences are also called "Symbols".
*/
//metadoc Sequence category Core


#define _GNU_SOURCE // for NAN macro
#include "IoSeq.h"
#include "IoState.h"
#include "IoCFunction.h"
#include "IoObject.h"
#include "IoNumber.h"
#include "IoMessage.h"
#include "IoList.h"
#include <ctype.h>
#include <errno.h>
#include <math.h> // for NAN macro
#ifdef _MSC_VER
static double dNaN()
{
	double a = 0, b = 0;
	return a / b;
}
#define NAN dNaN()
#endif
#ifndef NAN
#define NAN 0.0/0.0
#endif

#define DATA(self) ((UArray *)IoObject_dataPointer(self))


IoObject *IoSeq_rawAsSymbol(IoSeq *self)
{
	if (ISSYMBOL(self))
	{
		return self;
	}

	return IoState_symbolWithUArray_copy_(IOSTATE, DATA(self), 1);
}

IO_METHOD(IoSeq, with)
{
	/*doc Sequence with(aSequence, ...)
	Returns a new Sequence which is the concatination of the arguments.
	The returned sequence will have the same mutability status as the receiver.
	*/

	int n, argCount = IoMessage_argCount(m);
	UArray *ba = UArray_clone(DATA(self));

	for (n = 0; n < argCount; n ++)
	{
		IoSeq *v = IoMessage_locals_seqArgAt_(m, locals, n);
		UArray_append_(ba, DATA(v));
	}

	if (ISSYMBOL(self))
	{
		return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
	}

	return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, itemType)
{
	/*doc Sequence itemType
	Returns machine type of elements. 
	*/

	return IOSYMBOL(CTYPE_name(UArray_itemType(DATA(self))));
}

IO_METHOD(IoSeq, itemSize)
{
	/*doc Sequence itemSize
	Returns number of bytes in each element. 
	*/

	return IONUMBER(UArray_itemSize(DATA(self)));
}

IO_METHOD(IoSeq, encoding)
{
	/*doc Sequence encoding
	Returns the encoding of the elements. 
	*/

	return IOSYMBOL(CENCODING_name(UArray_encoding(DATA(self))));
}

IO_METHOD(IoSeq, asUTF8)
{
	/*doc Sequence asUTF8
	Returns a new copy of the receiver converted to utf8 encoding. 
	*/

	return IoSeq_newWithUArray_copy_(IOSTATE, UArray_asUTF8(DATA(self)), 0);
}

IO_METHOD(IoSeq, asUCS2)
{
	/*doc Sequence asUCS2
	Returns a new copy of the receiver converted to UCS2 (fixed character width UTF16) encoding. 
	*/

	return IoSeq_newWithUArray_copy_(IOSTATE, UArray_asUCS2(DATA(self)), 0);
}

IO_METHOD(IoSeq, asUCS4)
{
	/*doc Sequence asUCS4
	Returns a new copy of the receiver converted to UCS4 (fixed character width UTF32) encoding. 
	*/

	return IoSeq_newWithUArray_copy_(IOSTATE, UArray_asUCS4(DATA(self)), 0);
}

IO_METHOD(IoSeq, asFixedSizeType)
{
	/*doc Sequence asFixedSizeType
	Returns a new sequence with the receiver encoded in the 
	minimal fixed width text encoding that it's characters can fit 
	into (either, ascii, utf8, utf16 or utf32). 
	*/
	
	UArray *out = UArray_new();
	UArray_copy_(out, DATA(self));
	UArray_convertToFixedSizeType(out);	
	return IoSeq_newWithUArray_copy_(IOSTATE, out, 0);
}

IO_METHOD(IoSeq, asBinaryUnsignedInteger)
{
	/*doc Sequence asBinaryUnsignedInteger
	Returns a Number with the bytes of the receiver interpreted as a binary unsigned integer. Endian is same as machine.
	*/

	const void *bytes = UArray_bytes(DATA(self));
	size_t byteCount = UArray_size(DATA(self));

	if(byteCount == 1)
	{
		return IONUMBER(*((const uint8_t *)bytes));
	} 
	else if(byteCount == 2)
	{
		return IONUMBER(*((const uint16_t *)bytes));
	} 
	else if(byteCount == 4)
	{
		return IONUMBER(*((const uint32_t *)bytes));
	} 
	else 
	{
		IoState_error_(IOSTATE, m, "Sequence is %i bytes but only conversion of 1, 2, or 4 bytes is supported", byteCount);
	}

	return IONIL(self);
}

IO_METHOD(IoSeq, asBinarySignedInteger)
{
	/*doc Sequence asBinarySignedInteger
	Returns a Number with the bytes of the receiver interpreted as a binary signed integer. Endian is same as machine.
	*/

	const void *bytes = UArray_bytes(DATA(self));
	size_t byteCount = UArray_size(DATA(self));

	if(byteCount == 1)
	{
		return IONUMBER(*((const int8_t *)bytes));
	} 
	else if(byteCount == 2)
	{
		return IONUMBER(*((const int16_t *)bytes));
	} 
	else if(byteCount == 4)
	{
		return IONUMBER(*((const int32_t *)bytes));
	} 
	else 
	{
		IoState_error_(IOSTATE, m, "Sequence is %i bytes but only conversion of 1, 2, or 4 bytes is supported", byteCount);
	}

	return IONIL(self);
}


IO_METHOD(IoSeq, asBinaryNumber)
{
	/*doc Sequence asBinaryNumber
	Returns a Number containing the first 8 bytes of the
	receiver without casting them to a double. Endian is same as machine.
	*/

	IoNumber *byteCount = IoMessage_locals_valueArgAt_(m, locals, 0);
	size_t max = UArray_size(DATA(self));
	int bc = sizeof(double);
	double d = 0;

	if (!ISNIL(byteCount))
	{
		bc = IoNumber_asInt(byteCount);
	}

	if (max < bc)
	{
		IoState_error_(IOSTATE, m, "requested first %i bytes, but Sequence only contians %i bytes", bc, max);
	}

	memcpy(&d, UArray_bytes(DATA(self)), bc);
	return IONUMBER(d);
}

IO_METHOD(IoSeq, asSymbol)
{
	/*doc Sequence asSymbol
	Returns a immutable Sequence (aka Symbol) version of the receiver.
	*/

	return IoSeq_rawAsSymbol(self);
}

IO_METHOD(IoSeq, isSymbol)
{
	/*doc Sequence isSymbol
	Returns true if the receiver is a
	immutable Sequence (aka, a Symbol) or false otherwise.
	*/

	return IOBOOL(self, ISSYMBOL(self));
}

IO_METHOD(IoSeq, isMutable)
{
	/*doc Sequence isMutable
	Returns true if the receiver is a mutable Sequence or false otherwise.
	*/

	return IOBOOL(self, !ISSYMBOL(self));
}


IO_METHOD(IoSeq, print)
{
	/*doc Sequence print
	Prints the receiver as a string. Returns self.
	*/

	IoState_justPrintba_(IOSTATE, DATA(self));
	return self;
}

IO_METHOD(IoSeq, linePrint)
{
	/*doc Sequence linePrint
	Prints the Sequence and a newline character.
	*/

	IoState_justPrintba_(IOSTATE, DATA(self));
	IoState_justPrintln_(IOSTATE);
	return self;
}

IO_METHOD(IoSeq, isEmpty)
{
	/*doc Sequence isEmpty
	Returns true if the size of the receiver is 0, false otherwise.
	*/

	return IOBOOL(self, UArray_size(DATA(self)) == 0);
}

IO_METHOD(IoSeq, isZero)
{
	/*doc Sequence isZero
	Returns true if all elements are 0, false otherwise.
	*/

	return IOBOOL(self, UArray_isZero(DATA(self)));
}

IO_METHOD(IoSeq, size)
{
/*doc Sequence size
Returns the length in number of items (which may or may not
be the number of bytes, depending on the item type) of the receiver. For example,
<p>
<pre>	
"abc" size == 3
</pre>	
*/

	return IONUMBER(UArray_size(DATA(self)));
}

IO_METHOD(IoSeq, sizeInBytes)
{
	/*doc Sequence sizeInBytes
	Returns the length in bytes of the receiver.
	*/

	return IONUMBER(UArray_sizeInBytes(DATA(self)));
}


IO_METHOD(IoSeq, at)
{
	/*doc Sequence at(aNumber)
	Returns a value at the index specified by aNumber.
	Returns nil if the index is out of bounds.
	*/

	size_t i = IoMessage_locals_sizetArgAt_(m, locals, 0);
	UArray *a = DATA(self);

	//IOASSERT((i < UArray_size(DATA(self))), "index out of bounds");
	if(i >= UArray_size(DATA(self))) return IONIL(self);

	if(UArray_isFloatType(a))
	{
		return IONUMBER(UArray_doubleAt_(a, i));
	}
	else
	{
		return IONUMBER(UArray_longAt_(a, i));
	}
}

IO_METHOD(IoSeq, exclusiveSlice)
{
	/*doc Sequence exclusiveSlice(inclusiveStartIndex, exclusiveEndIndex)
	Returns a new string containing the subset of the
	receiver from the inclusiveStartIndex to the exclusiveEndIndex. The exclusiveEndIndex argument
	is optional. If not given, it is assumed to be one beyond the end of the string. 
	*/

	long fromIndex = IoMessage_locals_longArgAt_(m, locals, 0);
	long last = UArray_size(DATA(self));
	UArray *ba;

	if (IoMessage_argCount(m) > 1)
	{
		last = IoMessage_locals_longArgAt_(m, locals, 1);
	}

	ba = UArray_slice(DATA(self), fromIndex, last);

	if (ISSYMBOL(self))
	{
		return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
	}

	return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, inclusiveSlice)
{
	/*doc Sequence inclusiveSlice(inclusiveStartIndex, inclusiveEndIndex)
	Returns a new string containing the subset of the
	receiver from the inclusiveStartIndex to the inclusiveEndIndex. The inclusiveEndIndex argument
	is optional. If not given, it is assumed to be the end of the string. 
	*/

	long fromIndex = IoMessage_locals_longArgAt_(m, locals, 0);
	long last = UArray_size(DATA(self));
	UArray *ba;

	if (IoMessage_argCount(m) > 1)
	{
		last = IoMessage_locals_longArgAt_(m, locals, 1);
	}

	if (last == -1)
	{
		last = UArray_size(DATA(self));
	}
	else
	{
		last = last + 1;
	}
	
	ba = UArray_slice(DATA(self), fromIndex, last);

	if (ISSYMBOL(self))
	{
		return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
	}

	return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, between)
{
	/*doc Sequence between(aSequence, anotherSequence)
	Returns a new Sequence containing the bytes between the
	occurance of aSequence and anotherSequence in the receiver.
	*/

	long start = 0;
	long end = 0;
	IoSeq *fromSeq, *toSeq;

	fromSeq = (IoSeq *)IoMessage_locals_valueArgAt_(m, locals, 0);

	if (ISSEQ(fromSeq))
	{
		start = UArray_find_from_(DATA(self), DATA(fromSeq), 0) + IoSeq_rawSize(fromSeq);

		if (start == -1)
		{
			start = 0;
		}
	}
	else if (ISNIL(fromSeq))
	{
		start = 0;
	}
	else
	{
		IoState_error_(IOSTATE, m, "Nil or Sequence argument required for arg 0, not a %s",
					IoObject_name((IoObject *)fromSeq));
	}

	toSeq = (IoSeq *)IoMessage_locals_valueArgAt_(m, locals, 1);

	if (ISSEQ(toSeq))
	{
		end = UArray_find_from_(DATA(self), DATA(toSeq), start);
		if (end == -1) start = UArray_size(DATA(self));
	}
	else if (ISNIL(toSeq))
	{
		end = UArray_size(DATA(self));
	}
	else
	{
		IoState_error_(IOSTATE, m, "Nil or Sequence argument required for arg 1, not a %s",
					IoObject_name((IoObject *)toSeq));
	}

	{
		UArray *ba = UArray_slice(DATA(self), start, end);
		IoSeq *result = IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
		return result;
	}
}

// find ----------------------------------------------------------

IO_METHOD(IoSeq, findSeqs)
{
	/*doc Sequence findSeqs(listOfSequences, optionalStartIndex)
	Returns a object with two slots - an \"index\" slot which contains 
	the first occurrence of any of the sequences in listOfSequences found 
	in the receiver after the startIndex, and a \"match\" slot, which 
	contains a reference to the matching sequence from listOfSequences. 
	If no startIndex is specified, the search starts at index 0. 
	nil is returned if no occurences are found. 
	*/

	IoList *others = IoMessage_locals_listArgAt_(m, locals, 0);
	List *delims = IoList_rawList(others);
	long f = 0;
	long firstIndex = -1;
	int match = 0;

	if (IoMessage_argCount(m) > 1)
	{
		f = IoMessage_locals_longArgAt_(m, locals, 1);
	}

	{
		int index;

		LIST_FOREACH(delims, i, s,
			if (!ISSEQ((IoSeq *)s))
			{
				IoState_error_(IOSTATE, m, "requires Sequences as arguments, not %ss", IoObject_name((IoSeq *)s));
			}

			index = UArray_find_from_(DATA(self), DATA(((IoSeq *)s)), f);

			if(index != -1 && (firstIndex == -1 || index < firstIndex)) { firstIndex = index; match = i; }
		);
	}

	if (firstIndex == -1)
	{
		return IONIL(self);
	}
	else
	{
		IoObject *result = IoObject_new(IOSTATE);
		IoObject_setSlot_to_(result, IOSYMBOL("index"), IONUMBER(firstIndex));
		IoObject_setSlot_to_(result, IOSYMBOL("match"), (IoObject *)List_at_(delims, match));
		return result;
	}
}


IO_METHOD(IoSeq, findSeq)
{
	/*doc Sequence findSeq(aSequence, optionalStartIndex)
	Returns a number with the first occurrence of aSequence in
	the receiver after the startIndex. If no startIndex is specified,
	the search starts at index 0.
	nil is returned if no occurences are found. 
	*/

	IoSeq *otherSequence = IoMessage_locals_seqArgAt_(m, locals, 0);
	long f = 0;
	long index;

	if (IoMessage_argCount(m) > 1)
	{
		f = IoMessage_locals_longArgAt_(m, locals, 1);
	}

	index = UArray_find_from_(DATA(self), DATA(otherSequence), f);

	return (index == -1) ? IONIL(self) : IONUMBER(index);
}

IO_METHOD(IoSeq, reverseFindSeq)
{
	/*doc Sequence reverseFindSeq(aSequence, startIndex)
	Returns a number with the first occurrence of aSequence in
	the receiver before the startIndex. The startIndex argument is optional.
	By default reverseFind starts at the end of the string. Nil is
	returned if no occurrences are found. 
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	long from = UArray_size(DATA(self));
	long index;

	if (IoMessage_argCount(m) > 1)
	{
		from = IoMessage_locals_intArgAt_(m, locals, 1);
	}

	index = UArray_rFind_from_(DATA(self), DATA(other), from);

	if (index == -1)
	{
		return IONIL(self);
	}

	return IONUMBER((double)index);
}

IO_METHOD(IoSeq, beginsWithSeq)
{
	/*doc Sequence beginsWithSeq(aSequence)
	Returns true if the receiver begins with aSequence, false otherwise.
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);

	return IOBOOL(self, UArray_beginsWith_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, endsWithSeq)
{
	/*doc Sequence endsWithSeq(aSequence)
	Returns true if the receiver ends with aSequence, false otherwise. 
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	return IOBOOL(self, UArray_endsWith_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, contains)
{
	/*doc Sequence contains(aNumber)
	Returns true if the receiver contains an element equal in value to aNumber, false otherwise. 
	*/

	// will make this more efficient when Numbers are Arrays

	IoNumber *n = IoMessage_locals_numberArgAt_(m, locals, 0);

	UArray tmp = IoNumber_asStackUArray(n);
	return IOBOOL(self, UArray_contains_(DATA(self), &tmp));
}

IO_METHOD(IoSeq, containsSeq)
{
	/*doc Sequence containsSeq(aSequence)
	Returns true if the receiver contains the substring
	aSequence, false otherwise. 
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);

	return IOBOOL(self, UArray_contains_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, containsAnyCaseSeq)
{
	/*doc Sequence containsAnyCaseSeq(aSequence)
	Returns true if the receiver contains the aSequence
	regardless of casing, false otherwise. 
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	return IOBOOL(self, UArray_containsAnyCase_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, isLowercase)
{
	/*doc Sequence isLowercase
	Returns self if all the characters in the string are lower case.
	*/

	return IOBOOL(self, UArray_isLowercase(DATA(self)));
}

IO_METHOD(IoSeq, isUppercase)
{
	/*doc Sequence isUppercase
	Returns self if all the characters in the string are upper case.
	*/

	return IOBOOL(self, UArray_isUppercase(DATA(self)));
}

IO_METHOD(IoSeq, isEqualAnyCase)
{
	/*doc Sequence isEqualAnyCase(aSequence)
	Returns true if aSequence is equal to the receiver
	ignoring case differences, false otherwise.
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);

	return IOBOOL(self, UArray_equalsAnyCase_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, asNumber)
{
	/*doc Sequence asNumber
	Returns the receiver converted to a number.
	Initial whitespace is ignored.
	*/

	size_t size = UArray_size(DATA(self));
	char *s = (char *)UArray_bytes(DATA(self));
	char *endp;
	double d = strtod(s, &endp);

	if (size > 2 && s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		return IONUMBER(IoSeq_rawAsDoubleFromHex(self));
	}

	if (errno == ERANGE || endp == s)
	{
		return IONUMBER(NAN);
	}

	return IONUMBER(d);
}

IoList *IoSeq_whiteSpaceStrings(IoSeq *self, IoObject *locals, IoMessage *m)
{
	/*doc Sequence whiteSpaceStrings
	Returns a List of strings. Each string contains a different
	whitespace character.
	*/

	IoList *strings = IoList_new(IOSTATE);
	IoList_rawAppend_(strings, IOSYMBOL(" "));
	IoList_rawAppend_(strings, IOSYMBOL("\t"));
	IoList_rawAppend_(strings, IOSYMBOL("\n"));
	IoList_rawAppend_(strings, IOSYMBOL("\r"));
	return strings;
}

// split ---------------------------------------------------------------------

// this method is only used from split
IoList *IoSeq_stringListForArgs(IoSeq *self, IoObject *locals, IoMessage *m)
{
	if (IoMessage_argCount(m) == 0)
	{
		return IoSeq_whiteSpaceStrings(self, locals, m);
	}
	
	return IoMessage_evaluatedArgs(m, locals, m);
}

// this method is only used from split
List *IoSeq_byteArrayListForSeqList(IoSeq *self, IoObject *locals, IoMessage *m, IoList *seqs)
{
	List *args = IoList_rawList(seqs);
	List *list = List_new();

	LIST_FOREACH(args, i, s,
		if (!ISSEQ((IoSeq *)s))
		{
			List_free(list);
			IoState_error_(IOSTATE, m,
							"requires Sequences as arguments, not %ss",
							IoObject_name((IoSeq *)s));
		}

		List_append_(list, DATA(((IoSeq *)s)));
	);

	return list;
}

IoObject *IoSeq_splitToFunction(IoSeq *self,
								IoObject *locals,
								IoMessage *m,
								IoSplitFunction *func)
{
	IoList *output = IoList_new(IOSTATE);
	List *others = IoSeq_byteArrayListForSeqList(self, locals, m, IoSeq_stringListForArgs(self, locals, m));
	int i;

	for (i = 0; i < List_size(others); i ++)
	{
		if (UArray_size(List_at_(others, i)) == 0)
		{
			IoState_error_(IOSTATE, m, "empty string argument");
		}
	}

	{
		UArray othersArray = List_asStackAllocatedUArray(others);
		UArray *results = UArray_split_(DATA(self), &othersArray);

		for (i = 0; i < UArray_size(results); i ++)
		{
			UArray *ba = UArray_pointerAt_(results, i);
			IoObject *item = (*func)(IOSTATE, ba, 0);
			IoList_rawAppend_(output, item);
		}

		UArray_free(results);
	}

	List_free(others);
	return output;
}

IO_METHOD(IoSeq, split)
{
/*doc Sequence split(optionalArg1, optionalArg2, ...)
Returns a list containing the sub-sequences of the receiver divided by the given arguments.
If no arguments are given the sequence is split on white space.
Examples:
<pre>	
"a b c d" split == list("a", "b", "c", "d")
"a*b*c*d" split("*") == list("a", "b", "c", "d")
"a*b|c,d" split("*", "|", ",") == list("a", "b", "c", "d")
"a   b  c d" split == list("a", "", "", "", "b", "", "", "c", "", "d")
</pre>	
*/

	return IoSeq_splitToFunction(self, locals, m, IoSeq_newWithUArray_copy_);
}

IO_METHOD(IoSeq, splitAt)
{
	/*doc Sequence splitAt(indexNumber)
	Returns a list containing the two parts of the receiver as split at the given index.
	*/
	
	int index = IoMessage_locals_intArgAt_(m, locals, 0);
	IoList *splitSeqs = IoList_new(IOSTATE);
	index = UArray_wrapPos_(DATA(self), index);

	{
		const char *s = UArray_asCString(DATA(self));
		IoSeq *s1 = IoState_symbolWithCString_length_(IOSTATE, s, index);
		IoSeq *s2 = IoState_symbolWithCString_(IOSTATE, s + index);
		IoList_rawAppend_(splitSeqs, (IoObject *)s1);
		IoList_rawAppend_(splitSeqs, (IoObject *)s2);
	}

	return splitSeqs;
}

/* --- base -------------------------------------------------------------- */

IO_METHOD(IoSeq, fromBase)
{
	/*doc Sequence fromBase(aNumber)
	Returns a number with a base 10 representation of the receiver
	converted from the specified base. Only base 2 through 32 are currently supported.
	*/

	int base = IoMessage_locals_intArgAt_(m, locals, 0);
	char *s = CSTRING(self);
	unsigned long r;
	char *tail;
	errno = 0;
	r = strtoul(s, &tail, base);

	if (errno == EINVAL)
	{
		errno = 0;
		IoState_error_(IOSTATE, m, "conversion from base %i not supported", base);
	}
	else if (errno == ERANGE)
	{
		errno = 0;
		IoState_error_(IOSTATE, m, "resulting value \"%s\" was out of range", s);
	}
	else if (*s == 0 || *tail != 0 || errno != 0)
	{
		errno = 0;
		IoState_error_(IOSTATE, m, "conversion of \"%s\" to base %i failed", s, base);
	}

	return IONUMBER(r);
}

IO_METHOD(IoSeq, toBase)
{
	/*doc Sequence toBase(aNumber)
	Returns a Sequence containing the receiver(which is
	assumed to be a base 10 number) converted to the specified base.
	Only base 8 and 16 are currently supported. 
	*/

	const char * const table = "0123456789abcdefghijklmnopqrstuvwxyz";
	int base = IoMessage_locals_intArgAt_(m, locals, 0);
	unsigned long n = (unsigned long) IoSeq_asDouble(self);
	char buf[64], *ptr = buf + 64;

	if (base < 2 || base > 36)
	{
		IoState_error_(IOSTATE, m, "conversion to base %i not supported", base);
	}

	/* Build the converted string backwards. */
	*(--ptr) = '\0';

	if (n == 0)
	{
		*(--ptr) = '0';
	}
	else
	{
		do
		{
			*(--ptr) = table[n % base];
		}
		while ((n /= base) != 0);
	}

	return IoSeq_newWithCString_(IOSTATE, ptr);
}

// this function only called by IoSeq_foreach()
IO_METHOD(IoSeq, each)
{
	IoState *state = IOSTATE;
	IoObject *result = IONIL(self);
	IoMessage *doMessage = IoMessage_rawArgAt_(m, 0);
	size_t i;

	IoState_pushRetainPool(state);

	for (i = 0; i < UArray_size(DATA(self)); i ++)
	{
		IoState_clearTopPool(IOSTATE);

		if (UArray_isFloatType(DATA(self)))
		{
			result = IoMessage_locals_performOn_(doMessage, locals, IONUMBER(UArray_doubleAt_(DATA(self), i)));
		}
		else
		{
			result = IoMessage_locals_performOn_(doMessage, locals, IONUMBER(UArray_longAt_(DATA(self), i)));
		}

		if (IoState_handleStatus(IOSTATE))
		{
			goto done;
		}
	}

done:
		IoState_popRetainPoolExceptFor_(state, result);
	return result;
}


IO_METHOD(IoSeq, foreach)
{
/*doc Sequence foreach(optionalIndex, value, message)
For each element, set index to the index of the
element and value the element value and execute message. 
Example:
<pre>	
aSequence foreach(i, v, writeln("value at index ", i, " is ", v))
aSequence foreach(v, writeln("value ", v))
</pre>	
*/

	IoObject *result = IONIL(self);
	IoMessage *doMessage;

	IoSymbol *indexSlotName;
	IoSymbol *characterSlotName;

	size_t i;

	if (IoMessage_argCount(m) == 1)
	{
		return IoSeq_each(self, locals, m);
	}

	IoMessage_foreachArgs(m, self, &indexSlotName, &characterSlotName, &doMessage);

	IoState_pushRetainPool(IOSTATE);

	for (i = 0; i < UArray_size(DATA(self)); i ++)
	{
		IoState_clearTopPool(IOSTATE);

		if (indexSlotName)
		{
			IoObject_setSlot_to_(locals, indexSlotName, IONUMBER(i));
		}

		if(UArray_isFloatType(DATA(self)))
		{
			IoObject_setSlot_to_(locals, characterSlotName, IONUMBER(UArray_doubleAt_(DATA(self), i)));
		}
		else
		{
			IoObject_setSlot_to_(locals, characterSlotName, IONUMBER(UArray_longAt_(DATA(self), i)));
		}
		result = IoMessage_locals_performOn_(doMessage, locals, locals);

		if (IoState_handleStatus(IOSTATE))
		{
			goto done;
		}
	}
done:
		IoState_popRetainPoolExceptFor_(IOSTATE, result);
	return result;
}

IO_METHOD(IoSeq, asMessage)
{
	/*doc Sequence asMessage(optionalLabel)
	Returns the compiled message object for the string.
	*/

	IoSymbol *label;

	if (IoMessage_argCount(m) >= 1)
		label = IoMessage_locals_symbolArgAt_(m, locals, 0);
	else
		label = IOSYMBOL("[asMessage]");

	return IoMessage_newFromText_labelSymbol_(IOSTATE, CSTRING(self), label);
}

/*doc Sequence ..(aSequence)
	Returns a copy of the receiver with aSequence appended to it.
*/

IO_METHOD(IoSeq, cloneAppendSeq)
{
	IoObject *other = IoMessage_locals_valueArgAt_(m, locals, 0);
	UArray *ba;

	if (ISNUMBER(other))
	{
		other = IoNumber_justAsString((IoNumber *)other, (IoObject *)locals, m);
	}

	if (!ISSEQ(other))
	{
		IoState_error_(IOSTATE, m, "argument 0 to method '%s' must be a number or string, not a '%s'",
					CSTRING(IoMessage_name(m)),
					IoObject_name(other));
	}

	if (UArray_size(DATA(other)) == 0)
	{
		return self;
	}

	ba = UArray_clone(DATA(self));
	UArray_append_(ba, DATA(other));
	return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}


IO_METHOD(IoSeq, asMutable)
{
	/*doc Sequence asMutable
	Returns a mutable copy of the receiver. 
	*/

	return IoSeq_rawMutableCopy(self);
}

/* --- case ------------------------------------------------ */

IO_METHOD(IoSeq, asUppercase)
{
	/*doc Sequence asUppercase
	Returns a symbol containing the reveiver made uppercase. 
	*/

	UArray *ba = UArray_clone(DATA(self));
	UArray_toupper(ba);
	return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, asLowercase)
{
	/*doc Sequence asLowercase
	Returns a symbol containing the reveiver made Lowercase. 
	*/

	UArray *ba = UArray_clone(DATA(self));
	UArray_tolower(ba);
	return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}

/* --- path ------------------------------------------------ */


IO_METHOD(IoSeq, lastPathComponent)
{
	/*doc Sequence lastPathComponent
	Returns a string containing the receiver clipped up
	to the last path separator. 
	*/

	UArray *ba = UArray_lastPathComponent(DATA(self));
	return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, pathExtension)
{
	/*doc Sequence pathExtension
	Returns a string containing the receiver clipped up to the last period. 
	*/

	UArray *path = UArray_pathExtension(DATA(self));
	return IoState_symbolWithUArray_copy_(IOSTATE, path, 0);
}

IO_METHOD(IoSeq, fileName)
{
	/*doc Sequence fileName
	Returns the last path component sans the path extension.
*/

	UArray *path = UArray_fileName(DATA(self));
	return IoState_symbolWithUArray_copy_(IOSTATE, path, 0);
}

IO_METHOD(IoSeq, cloneAppendPath)
{
	/*doc Sequence cloneAppendPath(aSequence)
	Appends argument to a copy the receiver such that there is one
	and only one path separator between the two and returns the result.
	*/

	IoSeq *component = IoMessage_locals_seqArgAt_(m, locals, 0);
	UArray *ba = UArray_clone(DATA(self));
	UArray_appendPath_(ba, DATA(component));
	return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, pathComponent)
{
	/*doc Sequence pathComponent
	Returns a slice of the receiver before the last path separator as a symbol. 
	*/

	UArray *ba = UArray_clone(DATA(self));
	UArray_removeLastPathComponent(ba);
	return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
}

IO_METHOD(IoSeq, asOSPath)
{
	//doc Sequence asOSPath Returns a OS style path for an Io style path.
	return IoSeq_newSymbolWithUArray_copy_(IOSTATE, UArray_asOSPath(IoSeq_rawUArray(self)), 0);
}

IO_METHOD(IoSeq, asIoPath)
{
	//doc Sequence asIoPath Returns a Io style path for an OS style path.
	return IoSeq_newSymbolWithUArray_copy_(IOSTATE, UArray_asUnixPath(IoSeq_rawUArray(self)), 0);
}

// occurances

IO_METHOD(IoSeq, beforeSeq)
{
	/*doc Sequence beforeSeq(aSequence)
	Returns the slice of the receiver (as a Symbol) before
	aSequence or self if aSequence is not found.
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	long pos = UArray_find_(DATA(self), DATA(other));

	if (pos != -1)
	{
		UArray *ba = UArray_slice(DATA(self), 0, pos);

		if (ISSYMBOL(self))
		{
			return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
		}
		else
		{
			return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
		}
	}

	if (ISSYMBOL(self))
	{
		return self;
	}

	return IOCLONE(self);
}

IO_METHOD(IoSeq, afterSeq)
{
	/*doc Sequence afterSeq(aSequence)
	Returns the slice of the receiver (as a Symbol) after aSequence or
	nil if aSequence is not found. If aSequence is empty, the receiver
	(or a copy of the receiver if it is mutable) is returned.
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	long pos = UArray_find_(DATA(self), DATA(other));

	if (pos != -1)
	{
		UArray *ba = UArray_slice(DATA(self), pos + UArray_size(DATA(other)), UArray_size(DATA(self)));

		if (ISSYMBOL(self))
		{
			return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
		}
		else
		{
			return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
		}
	}

	return IONIL(self);
}

IO_METHOD(IoSeq, asCapitalized)
{
	/*doc Sequence asCapitalized
	Returns a copy of the receiver with the first charater made uppercase.
	*/

	/* need to fix for multi-byte characters */

	int firstChar = UArray_firstLong(DATA(self));
	int upperChar = toupper(firstChar);

	if (ISSYMBOL(self) && (firstChar == upperChar))
	{
		return self;
	}
	else
	{
		UArray *ba = UArray_clone(DATA(self));
		UArray_at_putLong_(ba, 0, upperChar);

		if (ISSYMBOL(self))
		{
			return IoState_symbolWithUArray_copy_(IOSTATE, ba, 0);
		}

		return IoSeq_newWithUArray_copy_(IOSTATE, ba, 0);
	}
}

IO_METHOD(IoSeq, occurancesOfSeq)
{
	/*doc Sequence occurancesOfSeq(aSeq)
	Returns count of aSeq in the receiver.
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	size_t count = UArray_count_(DATA(self), DATA(other));
	return IONUMBER(count);
}

IO_METHOD(IoSeq, asBase64)
{
	/*doc Sequence asBase64(optionalCharactersPerLine)
	Returns an immutable, base64 encoded (according to RFC 1421) version of self. 
	optionalCharactersPerLine describes the number of characters between line breaks and defaults to 0.
	*/
	int charsPerLine = 0;
	
	if (IoMessage_argCount(m) > 0)
	{
		charsPerLine = IoMessage_locals_intArgAt_(m, locals, 0);
	}
	
	return IoSeq_newWithUArray_copy_(IOSTATE, UArray_asBase64(IoSeq_rawUArray(self), charsPerLine), 0);
}

IO_METHOD(IoSeq, fromBase64)
{
	/*doc Sequence fromBase64
	Returns an immutable, base64 decoded (according to RFC 1421) version of self.
	*/
	
	return IoSeq_newWithUArray_copy_(IOSTATE, UArray_fromBase64(IoSeq_rawUArray(self)), 0);
}

IO_METHOD(IoSeq, interpolate)
{
	/*doc Sequence interpolate(ctx)
	Returns immutable copy of self with interpolateInPlace(ctx) passed to the copy.
	*/

	IoSeq *s = IoSeq_newWithUArray_copy_(IOSTATE, IoSeq_rawUArray(self), 1);
	IoSeq_interpolateInPlace(s, locals, m);
	return IoSeq_rawAsSymbol(s);
}

IO_METHOD(IoSeq, distanceTo)
{
	/*doc Sequence distanceTo(aSeq)
	Returns a number with the square root of the sum of the square 
	of the differences of the items between the sequences.
	*/

	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	double d;

	d = UArray_distanceTo_(DATA(self), DATA(other));
	return IONUMBER(d);
}

IO_METHOD(IoSeq, greaterThan_)
{
	/*doc Sequence greaterThan(aSeq) 
	Returns true if the receiver is greater than aSeq, false otherwise.
	*/
	
	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	return IOBOOL(self, UArray_greaterThan_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, lessThan_)
{
	/*doc Sequence lessThan(aSeq) 
	Returns true if the receiver is lass than aSeq, false otherwise.
	*/
	
	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	return IOBOOL(self, UArray_lessThan_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, greaterThanOrEqualTo_)
{
	/*doc Sequence greaterThanOrEqualTo(aSeq) 
	Returns true if the receiver is greater than or equal to aSeq, false otherwise.
	*/
	
	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	return IOBOOL(self, UArray_greaterThanOrEqualTo_(DATA(self), DATA(other)));
}

IO_METHOD(IoSeq, lessThanOrEqualTo_)
{
	/*doc Sequence lessThanOrEqualTo(aSeq) 
	Returns true if the receiver is lass than or equal to aSeq, false otherwise.
	*/
	
	IoSeq *other = IoMessage_locals_seqArgAt_(m, locals, 0);
	return IOBOOL(self, UArray_lessThanOrEqualTo_(DATA(self), DATA(other)));
}

// -----------------------------------------------------------

#define ASSTRUCT(type) if (!strcmp(mt, #type)) \
{ \
	int typeSize = sizeof(type ## _t); \
	IOASSERT(offset + typeSize <= size, "not enough data for struct"); \
	v = IONUMBER(*(type ## _t *)(data + offset)); \
	offset += typeSize; \
}

IO_METHOD(IoSeq, asStruct)
{
/*doc Sequence asStruct(memberList) 
For a sequence that contains the data for a raw memory data structure (as used in C),
this method can be used to extract it's members into an Object. The memberList argument
specifies the layout of the datastructure. It's form is:
<p>
list(memberType1, memberName1, memberType2, memberName2, ...)
<p>
Member types include: 
<pre>
int8, int16, int32, int64
uint8, uint16, uint32, uint64
float32, float64 
</pre>
Example:
<pre>
pointObject := structPointSeq asStruct(list("float32", "x", "float32", "y"))
</pre>
The output pointObject would contain x and y slots with Number objects.
*/
	
	IoObject *st = IoObject_new(IOSTATE);
	const unsigned char *data = UArray_bytes(DATA(self));
	size_t size = UArray_sizeInBytes(DATA(self));
	size_t offset = 0;
	List *members = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	int memberIndex;

	IOASSERT(List_size(members) % 2 == 0, "members list must be even number");

	for (memberIndex = 0; memberIndex < List_size(members) / 2 && offset < size; memberIndex ++)
	{
		IoSeq *memberType = List_at_(members, memberIndex*2);
		IoSeq *memberName = List_at_(members, memberIndex*2 + 1);
		char *mt;
		IoObject *v = NULL;

		IOASSERT(ISSEQ(memberType), "memberTypes must be strings");
		IOASSERT(ISSEQ(memberName), "memberNames must be strings");

		mt = CSTRING(memberType);

		ASSTRUCT(int8);
		ASSTRUCT(uint8);
		ASSTRUCT(int16);
		ASSTRUCT(uint16);
		ASSTRUCT(int32);
		ASSTRUCT(uint32);
		ASSTRUCT(int64);
		ASSTRUCT(uint64);
		ASSTRUCT(float32);
		ASSTRUCT(float64);

		IoObject_setSlot_to_(st, memberName, v);
	}

	return st;
}

#define WITHSTRUCT(type) if (!strcmp(mt, #type)) \
{ \
	int typeSize = sizeof(type ## _t); \
	*(type ## _t *)(data + offset) = CNUMBER(memberValue); \
	offset += typeSize; \
	continue; \
}

IO_METHOD(IoSeq, withStruct)
{
/*doc Sequence withStruct(memberList) 
This method is useful for producing a Sequence containing a raw datastructure with
the specified types and values. The memberList format is:
<p>
list(memberType1, memberName1, memberType2, memberName2, ...)
<p>
Member types include: 
<pre>
int8, int16, int32, int64
uint8, uint16, uint32, uint64
float32, float64 
</pre>
Example:
<pre>
pointStructSeq := Sequence withStruct(list("float32", 1.2, "float32", 3.5))
</pre>
The output pointStructSeq would contain 2 raw 32 bit floats.
*/
	
	List *members = IoList_rawList(IoMessage_locals_listArgAt_(m, locals, 0));
	int memberIndex;
	size_t maxSize = List_size(members) * 8;
	IoSeq *s = IoSeq_newWithData_length_(IOSTATE, malloc(maxSize), maxSize);
	unsigned char *data = IoSeq_rawBytes(s);
	size_t offset = 0;

	IOASSERT(List_size(members) % 2 == 0, "members list must be even number");

	for (memberIndex = 0; memberIndex < List_size(members) / 2 && offset < maxSize; memberIndex ++)
	{
		IoSeq *memberType = List_at_(members, memberIndex*2);
		IoSeq *memberValue = List_at_(members, memberIndex*2 + 1);
		char *mt;

		IOASSERT(ISSEQ(memberType), "memberTypes must be strings");
		IOASSERT(ISNUMBER(memberValue), "memberValues must be strings");

		mt = CSTRING(memberType);

		WITHSTRUCT(int8);
		WITHSTRUCT(uint8);
		WITHSTRUCT(int16);
		WITHSTRUCT(uint16);
		WITHSTRUCT(int32);
		WITHSTRUCT(uint32);
		WITHSTRUCT(int64);
		WITHSTRUCT(uint64);
		WITHSTRUCT(float32);
		WITHSTRUCT(float64);
	}

	IoSeq_rawSetSize_(s, offset);

	return s;
}

void IoSeq_addImmutableMethods(IoSeq *self)
{
	IoMethodTable methodTable[] = {
	{"itemType", IoSeq_itemType},
	{"itemSize", IoSeq_itemSize},
	{"encoding", IoSeq_encoding},
	{"asUTF8", IoSeq_asUTF8},
	{"asUCS2", IoSeq_asUCS2},
	{"asUCS4", IoSeq_asUCS4},
	{"asFixedSizeType", IoSeq_asFixedSizeType},

	{"asBinaryNumber", IoSeq_asBinaryNumber},
	{"asBinaryUnsignedInteger", IoSeq_asBinaryUnsignedInteger},
	{"asBinarySignedInteger", IoSeq_asBinaryUnsignedInteger},
	{"isSymbol", IoSeq_isSymbol},
	{"isMutable", IoSeq_isMutable},
	{"asSymbol", IoSeq_asSymbol},
	{"asString", IoSeq_asSymbol},
	{"asNumber", IoSeq_asNumber},
	{"whiteSpaceStrings", IoSeq_whiteSpaceStrings},
	{"print", IoSeq_print},
	{"linePrint", IoSeq_linePrint},
	{"size", IoSeq_size},
	{"sizeInBytes", IoSeq_sizeInBytes},
	{"isZero", IoSeq_isZero},
	{"isEmpty", IoSeq_isEmpty},
	{"at", IoSeq_at},
	{"exclusiveSlice", IoSeq_exclusiveSlice},
	{"exSlice", IoSeq_exclusiveSlice},
	{"inclusiveSlice", IoSeq_inclusiveSlice},
	{"inSlice", IoSeq_inclusiveSlice},
	{"between", IoSeq_between},
	{"betweenSeq", IoSeq_between},
	{"findSeqs", IoSeq_findSeqs},
	{"findSeq", IoSeq_findSeq},
	{"reverseFindSeq", IoSeq_reverseFindSeq},
	{"beginsWithSeq", IoSeq_beginsWithSeq},
	{"endsWithSeq", IoSeq_endsWithSeq},
	{"split", IoSeq_split},
	{"contains", IoSeq_contains},
	{"containsSeq", IoSeq_containsSeq},
	{"containsAnyCaseSeq", IoSeq_containsAnyCaseSeq},
	{"isLowercase", IoSeq_isLowercase},
	{"isUppercase", IoSeq_isUppercase},
	{"isEqualAnyCase", IoSeq_isEqualAnyCase},
	{"splitAt", IoSeq_splitAt},
	{"fromBase", IoSeq_fromBase},
	{"toBase", IoSeq_toBase},
	{"foreach", IoSeq_foreach},
	{"asMessage", IoSeq_asMessage},
	{"..", IoSeq_cloneAppendSeq},
	{"cloneAppendSeq", IoSeq_cloneAppendSeq},
	{"asMutable", IoSeq_asMutable},
	{"asBuffer", IoSeq_asMutable},

		// paths

	{"fileName", IoSeq_fileName},
	{"pathExtension", IoSeq_pathExtension},
	{"lastPathComponent", IoSeq_lastPathComponent},
	{"cloneAppendPath", IoSeq_cloneAppendPath},
	{"pathComponent", IoSeq_pathComponent},
	{"asOSPath", IoSeq_asOSPath},
	{"asIoPath", IoSeq_asIoPath},

	{"afterSeq",  IoSeq_afterSeq},
	{"beforeSeq", IoSeq_beforeSeq},

	{"asCapitalized", IoSeq_asCapitalized},
	{"asUppercase", IoSeq_asUppercase},
	{"asLowercase", IoSeq_asLowercase},
	{"with", IoSeq_with},
	{"occurancesOfSeq", IoSeq_occurancesOfSeq},
	{"interpolate", IoSeq_interpolate},
	{"distanceTo", IoSeq_distanceTo},

	{"asBase64", IoSeq_asBase64},
	{"fromBase64", IoSeq_fromBase64},

	{">", IoSeq_greaterThan_},
	{"<", IoSeq_lessThan_},
	{">=", IoSeq_greaterThanOrEqualTo_},
	{"<=", IoSeq_lessThanOrEqualTo_},

	{"asStruct", IoSeq_asStruct},
	{"withStruct", IoSeq_withStruct},
	{NULL, NULL},
	};

	IoObject_addMethodTable_(self, methodTable);
}