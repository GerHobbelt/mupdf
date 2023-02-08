//
// Convert legacy Qiqqa autotags files to JSON format.
//

#pragma message("TODO: implement this tool")

#include "mupdf/mutool.h"
#include "mupdf/fitz.h"
#include "mupdf/helpers/dir.h"
#include "mupdf/helpers/system-header-files.h"

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#if defined(_WIN32)
#include <windows.h>
#endif
#include <string>


static inline void memclr(void* ptr, size_t size)
{
	memset(ptr, 0, size);
}

static fz_context* ctx = NULL;
static fz_output* out = NULL;
static fz_stream* datafeed = NULL;

static void usage(void)
{
	fz_info(ctx,
		"muserver: Convert legacy Qiqqa autotags files to JSON format.\n"
		"\n"
		"Syntax: muserver [options] <autotagfile>\n"
		"\n"
		"Options:\n"
		"  -v      verbose (repeat to increase the chattiness of the application)\n"
		"  -q      quiet ~ not verbose at all\n"
		"\n"
		"  -V      display the version of this application and terminate\n"
	);
}


static void mu_drop_context(void)
{
	fz_close_output(ctx, out);
	fz_drop_output(ctx, out);
	out = NULL;
	fz_drop_stream(ctx, datafeed);
	datafeed = NULL;
	fz_drop_context(ctx); // also done here for those rare exit() calls inside the library code.
	ctx = NULL;
}

size_t decode_length(const uint8_t *bufptr, size_t &pos)
{
	size_t rv = 0;
	size_t shift = 0;
	uint8_t v = bufptr[pos];
	while (v & 0x80)
	{
		v &= ~0x80;
		rv += ((size_t)v) << shift;
		shift += 7;
		pos++;
		v = bufptr[pos];
	}
	pos++;
	rv += ((size_t)v) << shift;
	return rv;
}


extern "C" int
qiqqa_convert_legacy_autotags_main(int argc, const char** argv)
{
	int verbosity = 0;
	int c;
	const char* output = NULL;

	ctx = NULL;
	out = NULL;
	datafeed = NULL;

	ctx = fz_new_context(NULL, NULL, FZ_STORE_DEFAULT);
	if (!ctx)
	{
		fz_error(ctx, "cannot initialise MuPDF context");
		return EXIT_FAILURE;
	}

	fz_getopt_reset();
	while ((c = fz_getopt(argc, argv, "o:qvV")) != -1)
	{
		switch (c)
		{
		case 'o': output = fz_optarg; break;

		case 'q': verbosity = 0; break;

		case 'v': verbosity++; break;

		case 'V': fz_info(ctx, "muserver version %s/%s", FZ_VERSION, "SHA1"); return EXIT_FAILURE;

		default: usage(); return EXIT_FAILURE;
		}
	}

	atexit(mu_drop_context);

	if (fz_optind == argc)
	{
		fz_error(ctx, "No files specified to process\n\n");
		usage();
		return EXIT_FAILURE;
	}

	const char* datafilename = NULL;
	int errored = 0;

	fz_buffer *buf = NULL;

	fz_try(ctx)
	{
		if (!output || *output == 0 || !strcmp(output, "-"))
		{
			out = fz_stdout(ctx);
			output = NULL;
		}
		else
		{
			char fbuf[PATH_MAX];
			fz_format_output_path(ctx, fbuf, sizeof fbuf, output, 0);
			fz_normalize_path(ctx, fbuf, sizeof fbuf, fbuf);
			fz_sanitize_path(ctx, fbuf, sizeof fbuf, fbuf);
			out = fz_new_output_with_path(ctx, fbuf, 0);
		}

		while (fz_optind < argc)
		{
			// load a datafile if we already have a script AND we're in "template mode".
			datafilename = argv[fz_optind++];

			if (fz_path_is_directory(ctx, datafilename))
			{
				datafilename = fz_asprintf(ctx, "%s/Qiqqa.autotags", *datafilename ? datafilename : ".");
			}
			else
			{
				datafilename = fz_strdup(ctx, datafilename);
			}

			int64_t suggested_filesize = fz_stat_size(ctx, datafilename);
			if (suggested_filesize < 0)
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot determine the size of autotags file: %s", datafilename);

			datafeed = fz_open_file(ctx, datafilename);
			if (datafeed == NULL)
				fz_throw(ctx, FZ_ERROR_GENERIC, "cannot open datafile: %s", datafilename);

			buf = fz_read_all(ctx, datafeed, suggested_filesize);
			if (buf == NULL)
				fz_throw(ctx, FZ_ERROR_GENERIC, "Failed to read the data in the autotags file: %s", datafilename);

			uint8_t *bufptr = NULL;
			size_t bufsize = fz_buffer_storage(ctx, buf, &bufptr);

			fz_drop_stream(ctx, datafeed);
			datafeed = NULL;

			//
			// AITags format:
			//
			//   [ProtoMember(1)]
			//   private DateTime timestamp_generated;
			//   [ProtoMember(2)]
			//   private MultiMapSet<string, string> ai_tags_with_documents; // tag -> documents
			//   [ProtoMember(3)]
			//   private MultiMapSet<string, string> ai_documents_with_tags; // document -> tags
			//
			// where
			//
			//   // Provides a multiple map, where a given key points to several items.  Note that the items will be unique.
			//   [Serializable]
			//   [ProtoContract]
			//   public class MultiMapSet<KEY, VALUE>
			//   {
			//  	private static HashSet<VALUE> EMPTY_VALUE_SET = new HashSet<VALUE>();
			//  
			//  	[ProtoMember(1)]
			//  	private Dictionary<KEY, HashSet<VALUE>> data = new Dictionary<KEY, HashSet<VALUE>>();
			//
			//      ...
			//   }
			//
			//
			//   [Serializable]
			//   ...
			//   public class Dictionary<TKey, TValue>: ...
			//   {
			//   	private struct Entry
			//   	{
			//   		public int hashCode;
			//   		public int next;
			//   		public TKey key;
			//   		public TValue value;
			//   	}
			//      ...
			//   }
			//
			//
			//   [Serializable]
			//   ...
			//   public class HashSet<T>: ...
			//   {
			//   	internal struct ElementCount
			//   	{
			//   		internal int uniqueCount;
			//   		internal int unfoundCount;
			//   	}
			//   
			//   	internal struct Slot
			//   	{
			//   		internal int hashCode;
			//   		internal int next;
			//   		internal T value;
			//   	}
			//      ...
			//   }
			//
			// 
			//   [Serializable]
			//   public struct DateTime: ...
			//   {
			//   	internal struct FullSystemTime
			//   	{
			//   		internal ushort wYear;
			//   		internal ushort wMonth;
			//   		internal ushort wDayOfWeek;
			//   		internal ushort wDay;
			//   		internal ushort wHour;
			//   		internal ushort wMinute;
			//   		internal ushort wSecond;
			//   		internal ushort wMillisecond;
			//   		internal long hundredNanoSecond;
			//   
			//   	private const long TicksPerMillisecond = 10000L;
			//   	private const long TicksPerSecond = 10000000L;
			//   	private const long TicksPerMinute = 600000000L;
			//   	private const long TicksPerHour = 36000000000L;
			//   	private const long TicksPerDay = 864000000000L;
			//   	private const int MillisPerSecond = 1000;
			//   	private const int MillisPerMinute = 60000;
			//   	private const int MillisPerHour = 3600000;
			//   	private const int MillisPerDay = 86400000;
			//   	private const int DaysPerYear = 365;
			//   	private const int DaysPer4Years = 1461;
			//   	private const int DaysPer100Years = 36524;
			//   	private const int DaysPer400Years = 146097;
			//   	private const int DaysTo1601 = 584388;
			//   	private const int DaysTo1899 = 693593;
			//   	internal const int DaysTo1970 = 719162;
			//   	private const int DaysTo10000 = 3652059;
			//   	internal const long MinTicks = 0L;
			//   	internal const long MaxTicks = 3155378975999999999L;
			//   	private const long MaxMillis = 315537897600000L;
			//   	private const long FileTimeOffset = 504911232000000000L;
			//   	private const long DoubleDateOffset = 599264352000000000L;
			//   	private const long OADateMinAsTicks = 31241376000000000L;
			//   	private const double OADateMinAsDouble = -657435.0;
			//   	private const double OADateMaxAsDouble = 2958466.0;
			//   	private const int DatePartYear = 0;
			//   	private const int DatePartDayOfYear = 1;
			//   	private const int DatePartMonth = 2;
			//   	private const int DatePartDay = 3;
			//   	internal static readonly bool s_isLeapSecondsSupportedSystem = SystemSupportLeapSeconds();
			//   	private static readonly int[] DaysToMonth365 = new int[13]
			//   	{
			//   		0, 31, 59, 90, 120, 151, 181, 212, 243, 273,
			//   			304, 334, 365
			//   	};
			//   	private static readonly int[] DaysToMonth366 = new int[13]
			//   	{
			//   		0, 31, 60, 91, 121, 152, 182, 213, 244, 274,
			//   			305, 335, 366
			//   	};
			//   
			//   	public static readonly DateTime MinValue = new DateTime(0L, DateTimeKind.Unspecified);
			//   	public static readonly DateTime MaxValue = new DateTime(3155378975999999999L, DateTimeKind.Unspecified);
			//   	private const ulong TicksMask = 4611686018427387903uL;
			//   	private const ulong FlagsMask = 13835058055282163712uL;
			//   	private const ulong LocalMask = 9223372036854775808uL;
			//   	private const long TicksCeiling = 4611686018427387904L;
			//   	private const ulong KindUnspecified = 0uL;
			//   	private const ulong KindUtc = 4611686018427387904uL;
			//   	private const ulong KindLocal = 9223372036854775808uL;
			//   	private const ulong KindLocalAmbiguousDst = 13835058055282163712uL;
			//   	private const int KindShift = 62;
			//   	private const string TicksField = "ticks";
			//   	private const string DateDataField = "dateData";
			//   
			//   	private ulong dateData;
			//   	internal long InternalTicks => (long)(dateData & 0x3FFFFFFFFFFFFFFFL);
			//   	private ulong InternalKind => dateData & 0xC000000000000000uL;
			//   
			//   	public static DateTime UtcNow
			//   	{
			//   		[SecuritySafeCritical]
			//   		[__DynamicallyInvokable]
			//   		get
			//   		{
			//   			long num = 0L;
			//   			if (s_isLeapSecondsSupportedSystem)
			//   			{
			//   				FullSystemTime time = default(FullSystemTime);
			//   				GetSystemTimeWithLeapSecondsHandling(ref time);
			//   				return CreateDateTimeFromSystemTime(ref time);
			//   			}
			//   
			//   			num = GetSystemTimeAsFileTime();
			//   			return new DateTime((ulong)(num + 504911232000000000L) | 0x4000000000000000uL);
			//   		}
			//   	}
			//
			//
			// 		public int Second
			//   	{
			//   		[__DynamicallyInvokable]
			//   		get
			//   		{
			//   			return (int)(InternalTicks / 10000000 % 60);
			//   		}
			//   	}
			// 
			//
			//   	public DateTime(long ticks)
			//   	{
			//   		if (ticks < 0 || ticks > 3155378975999999999L)
			//   		{
			//   			throw new ArgumentOutOfRangeException("ticks", Environment.GetResourceString("ArgumentOutOfRange_DateTimeBadTicks"));
			//   		}
			//   
			//   		dateData = (ulong)ticks;
			//   	}
			//
			//
			//   	//
			//   	// Summary:
			//   	//     Deserializes a 64-bit binary value and recreates an original serialized System.DateTime
			//   	//     object.
			//   	//
			//   	// Parameters:
			//   	//   dateData:
			//   	//     A 64-bit signed integer that encodes the System.DateTime.Kind property in a 2-bit
			//   	//     field and the System.DateTime.Ticks property in a 62-bit field.
			//   	//
			//   	// Returns:
			//   	//     An object that is equivalent to the System.DateTime object that was serialized
			//   	//     by the System.DateTime.ToBinary method.
			//   	//
			//   	// Exceptions:
			//   	//   T:System.ArgumentException:
			//   	//     dateData is less than System.DateTime.MinValue or greater than System.DateTime.MaxValue.
			//   	[__DynamicallyInvokable]
			//   	public static DateTime FromBinary(long dateData)
			//   	{
			//   		if ((dateData & long.MinValue) != 0L)
			//   		{
			//   			long num = dateData & 0x3FFFFFFFFFFFFFFFL;
			//   			if (num > 4611685154427387904L)
			//   			{
			//   				num -= 4611686018427387904L;
			//   			}
			//   
			//   			bool isAmbiguousLocalDst = false;
			//   			long ticks;
			//   			if (num < 0)
			//   			{
			//   				ticks = TimeZoneInfo.GetLocalUtcOffset(MinValue, TimeZoneInfoOptions.NoThrowOnInvalidTime).Ticks;
			//   			} else if (num > 3155378975999999999L)
			//   			{
			//   				ticks = TimeZoneInfo.GetLocalUtcOffset(MaxValue, TimeZoneInfoOptions.NoThrowOnInvalidTime).Ticks;
			//   			} else
			//   			{
			//   				DateTime time = new DateTime(num, DateTimeKind.Utc);
			//   				bool isDaylightSavings = false;
			//   				ticks = TimeZoneInfo.GetUtcOffsetFromUtc(time, TimeZoneInfo.Local, out isDaylightSavings, out isAmbiguousLocalDst).Ticks;
			//   			}
			//   
			//   			num += ticks;
			//   			if (num < 0)
			//   			{
			//   				num += 864000000000L;
			//   			}
			//   
			//   			if (num < 0 || num > 3155378975999999999L)
			//   			{
			//   				throw new ArgumentException(Environment.GetResourceString("Argument_DateTimeBadBinaryData"), "dateData");
			//   			}
			//   
			//   			return new DateTime(num, DateTimeKind.Local, isAmbiguousLocalDst);
			//   		}
			//   
			//   		return FromBinaryRaw(dateData);
			//   	}
			//
			//   
			//   	internal static DateTime FromBinaryRaw(long dateData)
			//   	{
			//   		long num = dateData & 0x3FFFFFFFFFFFFFFFL;
			//   		if (num < 0 || num > 3155378975999999999L)
			//   		{
			//   			throw new ArgumentException(Environment.GetResourceString("Argument_DateTimeBadBinaryData"), "dateData");
			//   		}
			//   
			//   		return new DateTime((ulong)dateData);
			//   	}
			//
			//   

			uint8_t timestamp[16] ={0};

			// timestamp field:
			size_t pos = 0;
			if (bufptr[pos] != 0x0A)
				fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

			pos++;
			size_t offset = pos;
			size_t timestamp_part_len = decode_length(bufptr, offset);
			if (timestamp_part_len > 16)
				fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

			memcpy(timestamp, bufptr + offset, timestamp_part_len);

			pos = offset + timestamp_part_len;

			for (int multimap_id = 1; multimap_id <= 2; multimap_id++)
			{
				size_t bundle_len = 0;
				size_t end_of_multimap_pos = bufsize;

				if (multimap_id == 1)
				{
					if (bufptr[pos] != 0x12)
						fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

					pos++;
					offset = pos;
					bundle_len = decode_length(bufptr, offset);
					if (bundle_len > bufsize)
						fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

					end_of_multimap_pos = offset + bundle_len;

					pos = offset;
				}
				else
				{
					if (bufptr[pos] != 0x1A)
						fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

					pos++;
					offset = pos;
					bundle_len = decode_length(bufptr, offset);
					if (bundle_len > bufsize)
						fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

					end_of_multimap_pos = offset + bundle_len;

					pos = offset;
				}

				// load multimap:
				while (pos < end_of_multimap_pos)
				{
					// tag dictionary #1: 
					if (bufptr[pos] != 0x0A)
						fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

					pos++;
					offset = pos;
					size_t multimap_dictionary1_len = decode_length(bufptr, offset);
					if (multimap_dictionary1_len > bufsize)
						fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

					size_t end_of_hashtable_pos = offset + multimap_dictionary1_len;

					pos = offset;

					// tag dictionary #1: tag -> dictionary entry: key (tag)
					if (bufptr[pos] != 0x0A)
						fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

					pos++;
					offset = pos;
					size_t dict_entry_key_len = decode_length(bufptr, offset);
					if (dict_entry_key_len > bufsize)
						fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

					char strbuf[4000];
					memcpy(strbuf, bufptr + offset, dict_entry_key_len);
					strbuf[dict_entry_key_len] = 0;

					pos = offset + dict_entry_key_len;

					size_t slot_count = 0;

					// load hash table for the given key/tag:
					while (pos < end_of_hashtable_pos)
					{
						// dictionary entry: hash value
						if (bufptr[pos] != 0x12)
							fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

						pos++;
						offset = pos;
						size_t doc_hash_value_len = decode_length(bufptr, offset);
						if (doc_hash_value_len > bufsize)
							fz_throw(ctx, FZ_ERROR_GENERIC, "Unexpected format byte %.*H at offset %zu in the autotags file: %s", 1, bufptr + pos, pos, datafilename);

						memcpy(strbuf, bufptr + offset, doc_hash_value_len);
						strbuf[doc_hash_value_len] = 0;

						slot_count++;

						pos = offset + doc_hash_value_len;
					}
				}
			}

			if (buf)
			{
				fz_drop_buffer(ctx, buf);
				buf = NULL;
			}
		}
	}
	fz_catch(ctx)
	{
		if (datafeed)
		{
			fz_drop_stream(ctx, datafeed);
			datafeed = NULL;
		}

		if (buf)
		{
			fz_drop_buffer(ctx, buf);
			buf = NULL;
		}

		fz_error(ctx, "Failure while processing %q: %s", datafilename, fz_caught_message(ctx));

		errored = 1;
	}

	fz_free(ctx, datafilename);
	fz_flush_warnings(ctx);
	mu_drop_context();

	return errored;
}
