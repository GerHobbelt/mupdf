-- SQLite script
--
-- run as:
--     cat *.sql | sqlite3 -table
--

.echo on
.changes on
.eqp full

.vfslist


--------------------------------------------------------------

.open "qiqqa.next-gen.library"

PRAGMA journal_mode = WAL;
PRAGMA journal_mode;

PRAGMA main.synchronous = NORMAL;
PRAGMA main.synchronous;

-- changing page_size has no effect when database is in WAL mode, so we need to change that first:
PRAGMA journal_mode = MEMORY;

PRAGMA main.page_size = 16384;
PRAGMA main.page_size;

-- changing page_size has no effect until database is VACUUMed.
VACUUM;

-- and now that the page size has changed, revert to WAL mode:
PRAGMA journal_mode = WAL;
PRAGMA journal_mode;

PRAGMA main.application_id = 0x51516462; 		-- "QQdb"
PRAGMA main.application_id;

PRAGMA main.user_version = 2;
PRAGMA main.user_version;

--------------------------------------------------------------

--
-- drop all tables (as per https://stackoverflow.com/questions/525512/drop-all-tables-command )
--
PRAGMA writable_schema = 1;
delete from sqlite_master where type in ('table', 'index', 'trigger');
PRAGMA writable_schema = RESET;
VACUUM;
PRAGMA INTEGRITY_CHECK;
--.quit

--------------------------------------------------------------

-- close & re-open database: this should automatically reload the page_size, wal mode, etc. db settings we have previously set up for this database:
.open "qiqqa.next-gen.library"

PRAGMA journal_mode;
PRAGMA main.synchronous;
PRAGMA main.page_size;
PRAGMA main.application_id;
PRAGMA main.user_version;

--.quit

-- https://www.sqlite.org/foreignkeys.html
PRAGMA foreign_keys = ON;

--------------------------------------------------------------

-- # BibTeX format
--
-- The following are the standard entry types, along with their required and
-- optional fields, that are used by the standard bibliography styles. The
-- fields within each class (required or optional) are listed in order of
-- occurrence in the output, except that a few entry types may perturb the
-- order slightly, depending on what fields are missing. These entry types
-- are similar to those adapted by Brian Reid from the classification scheme
-- of van Leunen [4] for use in the Scribe system. The meanings of the
-- individual fields are explained in the next section. Some nonstandard
-- bibliography styles may ignore some optional fields in creating the
-- reference. Remember that, when used in the bib file, the entry-type name
-- is preceded by an @ character.
--
-- article
--     An article from a journal or magazine. Required fields: author, title,
--     journal, year. Optional fields: volume, number, pages, month, note.
-- book
--     A book with an explicit publisher. Required fields: author or editor,
--     title, publisher, year. Optional fields: volume or number, series,
--     address, edition, month, note.
-- booklet
--     A work that is printed and bound, but without a named publisher
--     or sponsoring institution. Required field: title. Optional fields:
--     author, howpublished, address, month, year, note.
-- conference
--     The same as INPROCEEDINGS, included for Scribe compatibility.
-- inbook
--     A part of a book, which may be a chapter (or section or whatever)
--     and/or a range of pages. Required fields: author or editor, title,
--     chapter and/or pages, publisher, year. Optional fields: volume or
--     number, series, type, address, edition, month, note.
-- incollection
--     A part of a book having its own title. Required fields: author, title,
--     booktitle, publisher, year. Optional fields: editor, volume or number,
--     series, type, chapter, pages, address, edition, month, note.
-- inproceedings
--     An article in a conference proceedings. Required fields: author,
--     title, booktitle, year. Optional fields: editor, volume or number,
--     series, pages, address, month, organization, publisher, note.
-- manual
--     Technical documentation. Required field: title. Optional fields:
--     author, organization, address, edition, month, year, note.
-- mastersthesis
--     A Master's thesis. Required fields: author, title, school,
--     year. Optional fields: type, address, month, note.
-- misc
--     Use this type when nothing else fits. Required fields: none. Optional
--     fields: author, title, howpublished, month, year, note.
-- phdthesis
--     A PhD thesis. Required fields: author, title, school, year. Optional
--     fields: type, address, month, note.
-- proceedings
--     The proceedings of a conference. Required fields: title,
--     year. Optional fields: editor, volume or number, series, address,
--     month, organization, publisher, note.
-- techreport
--     A report published by a school or other institution, usually numbered
--     within a series. Required fields: author, title, institution,
--     year. Optional fields: type, number, address, month, note.
-- unpublished
--     A document having an author and title, but not formally
--     published. Required fields: author, title, note. Optional fields:
--     month, year.
--
-- article: academic journal
-- communication
-- conference publication: INPROCEEDINGS
-- Diploma thesis
-- Doctoral thesis
-- Habilitation thesis
-- hearing
-- legal case and case notes: CASE
-- legislation: BILL
-- Licentiate thesis
-- magazine
-- manuscript
-- Masters thesis
-- newspaper
-- patent
-- Ph.D. thesis
-- report / technical report
-- unpublished
-- periodical: ARTICLE
-- thesis
-- book
-- collection: BOOK / INBOOK
--
-- monographic
-- datasheet
-- Application Note: APPNOTE
-- abstract / summary
-- review
-- pamphlet
-- patent
-- case (law)
-- statute
-- generic
-- [private] communication
-- electronic [message]
-- letter [to the editor / ...]
-- booklet
-- periodical
-- inconference
--
--
--
--
--
-- ## Fields
--
-- Below is a description of all fields recognized by the standard
-- bibliography styles. An entry can also contain other fields, which are
-- ignored by those styles.
--
-- address
--     Usually the address of the publisher or other type of institution. For
--     major publishing houses, van Leunen recommends omitting the
--     information entirely. For small publishers, on the other hand,
--     you can help the reader by giving the complete address.
-- annote
--     An annotation. It is not used by the standard bibliography styles,
--     but may be used by others that produce an annotated bibliography.
-- author
--     The name(s) of the author(s), in the format described in the LATEX
--     book.
-- booktitle
--     Title of a book, part of which is being cited. See the LATEX book
--     for how to type titles. For book entries, use the title field instead.
-- chapter
--     A chapter (or section or whatever) number.
-- crossref
--     The database key of the entry being cross referenced.
-- edition
--     The edition of a book--for example, ``Second''. This should be an
--     ordinal, and should have the first letter capitalized, as shown here;
--     the standard styles convert to lower case when necessary.
-- editor
--     Name(s) of editor(s), typed as indicated in the LATEX book. If there
--     is also an author field, then the editor field gives the editor of
--     the book or collection in which the reference appears.
-- howpublished
--     How something strange has been published. The first word should
--     be capitalized.
-- institution
--     The sponsoring institution of a technical report.
-- journal
--     A journal name. Abbreviations are provided for many journals; see
--     the Local Guide.
-- key
--     Used for alphabetizing, cross referencing, and creating a label when
--     the ``author'' information (described in Section 4) is missing. This
--     field should not be confused with the key that appears in the \cite
--     command and at the beginning of the database entry.
-- month
--     The month in which the work was published or, for an unpublished work,
--     in which it was written. You should use the standard three-letter
--     abbreviation, as described in Appendix B.1.3 of the LATEX book.
-- note
--     Any additional information that can help the reader. The first word
--     should be capitalized.
-- number
--     The number of a journal, magazine, technical report, or of a work in
--     a series. An issue of a journal or magazine is usually identified
--     by its volume and number; the organization that issues a technical
--     report usually gives it a number; and sometimes books are given
--     numbers in a named series.
-- organization
--     The organization that sponsors a conference or that publishes
--     a manual.
-- pages
--     One or more page numbers or range of numbers, such as 42-111 or
--     7,41,73-97 or 43+ (the `+' in this last example indicates pages
--     following that don't form a simple range). To make it easier to
--     maintain Scribe-compatible databases, the standard styles convert
--     a single dash (as in 7-33) to the double dash used in TEX to denote
--     number ranges (as in 7-33).
-- publisher
--     The publisher's name.
-- school
--     The name of the school where a thesis was written.
-- series
--     The name of a series or set of books. When citing an entire book, the
--     the title field gives its title and an optional series field gives the
--     name of a series or multi-volume set in which the book is published.
-- title
--     The work's title, typed as explained in the LATEX book.
-- type
--     The type of a technical report--for example, ``Research Note''.
-- volume
--     The volume of a journal or multivolume book.
-- year
--     The year of publication or, for an unpublished work, the year it was
--     written. Generally it should consist of four numerals, such as 1984,
--     although the standard styles can handle any year whose last four
--     nonpunctuation characters are numerals, such as `(about 1984)'.
--



--
-- used to track edit history and publication history both:
--
CREATE TABLE metadata_root(
	id INTEGER PRIMARY KEY
);

--
-- the main document metadata store
--
CREATE TABLE metadata_record(
	id INTEGER PRIMARY KEY,
	root INTEGER REFERENCES metadata_root,					-- just a helper so we can filter/extract the revision history subgraph for the given document

	key TEXT,                       -- semi-unique key used as document identifier in paper references, e.g. "Knuth69A". Can and will be regenerated; MAY NOT be unique when records have been imported from external sources.
	type TEXT,                      -- article, masterthesis,book,techreport,... - See also the Putnam bibutils for an exhaustive list as we'll be using those tools to straighten up these records when incoming.
	address TEXT,					-- see BibTeX format text further above...
	annote TEXT,					-- An annotation. see BibTeX format text further above...
	author TEXT,					-- The name(s) of the author(s).   Flattened column; for the real set of authors, see the `authors4metarecord` relation table.
--	booktitle TEXT, <-- obtained via `grouping4metarecord` relation
	chapter TEXT,					-- TEXT format as chapter/section references CAN be alphanumeric, e.g. "(appendix) A1.3"
--	crossref TEXT,
	edition TEXT,
	editor TEXT,					-- Name(s) of editor(s).   Flattened column; for the real set of editors, see the `authors4metarecord` relation table.
	howpublished TEXT,
	institution TEXT,
	journal TEXT,
--	month INTEGER,					-- NULL or 1..12
	note TEXT,						-- Any additional information that can help the reader.
	number TEXT,					-- The number of a journal, magazine, technical report, or of a work in a series.
	organization TEXT,
	pages TEXT,						-- One or more page numbers or range of numbers, such as 42-111 or 7,41,73-97 or 43+. Used when this is part of a larger publication.
	page_count INTEGER,             -- the page count according to public info / consensus or the actual page count of the electronic docunment we have in store. These would be stored in separate records
	publisher TEXT,
	school TEXT,
	series TEXT,
	title TEXT,
	subtype TEXT,					-- The type of a technical report--for example, "Research Note".
	volume TEXT,
	year INTEGER,					-- The year of publication or, for an unpublished work, the year it was written. Generally it should consist of four numerals, such as 1984.

	abstract TEXT,
	doi TEXT,
	issue TEXT,
	language TEXT,

	keywords TEXT,
	label TEXT,
	genre TEXT,
	attachments TEXT,				-- File Attachments (local, not URL)

	num_volumes INTEGER,			-- Number of Volumes
	orig_publication BOOLEAN,		-- Is this the Original Publication, i.e. place where it got published first?
	reviewed BOOLEAN,				-- Reviewed Item
	reprint_status TEXT,			-- Reprint Edition
	section TEXT,

	isbn TEXT,						-- ISBN
	issn TEXT,						-- ISSN
	isi_delivery_number TEXT,
	isi_article_identifier TEXT,
	pmid TEXT,						-- PMID / PMC
	lccn TEXT,

	url TEXT,						-- list of URLs where this document has been found.   Flattened column; for the real set of URLs, see the `urls4metarecord` relation table.

	short_title TEXT,
	secondary_title TEXT,
	tertiary_title TEXT,
	translated_title TEXT,

	conference TEXT,				-- Name(s) of conference.
	status TEXT,					-- preprint, published, retracted, ...
	call_number TEXT,				-- call/case number (hearing)
	refnum TEXT,
	addressee TEXT,					-- addressed to ([private] communication)

	preferred_key TEXT,             -- **suggested** semi-unique key used as document identifier (key); when not unique itself, this will be used, IFF possible, as the base for the (re)generated key.
--	day INTEGER,					-- NULL or 1..31
	timestamp INTEGER,				-- NULL or 64-bit timestamp (as per `lib::eternal_timestamp`): publication/discovery date.

	date_filed REAL,				-- date when filed (patent, hearing)

	how_published TEXT,

	filename TEXT,					-- original filename (when imported into the library)
	file_present BOOLEAN,
	content_hash TEXT,				-- hash of `filename`
	content_hash_SHA1B TEXT,		-- "classic" hash of `filename` (OBSOLETE, NOT UNIQUE)

	contents TEXT
);







--
-- individual or corporate entity/organization:
-- both can be authors, ec., so we've folded both types into a single record structure.
--
CREATE TABLE party(
	id INTEGER PRIMARY KEY,

	name TEXT,
	address TEXT,
	language TEXT,

	-- individual:

	email TEXT,
	phone TEXT,

	-- organization / corporation:

	secondary_name TEXT,
	country TEXT,
	url TEXT,
	location TEXT		-- geocoordinate of campus / building
);



-- definition table for the `purpose` columns elsewhere. (An 'enum', in 'C' parlance.)
CREATE TABLE purpose(
	id INTEGER PRIMARY KEY,
	category INTEGER,   -- so we encode for which relation this MAY be used: author, affiliation, revision/edit, ...
	purpose TEXT
						-- author, primary author, subsidiary author, sponsor, editor, conference editor, series editor, investigator, translator, ..., 
						-- publisher, sponsor, conference organizer, *corporate author*, ...
 						-- employer, sponsor, ..., colleague, project lead, ...
                        -- DB commit specific purposes: duplicate, (re)published, original, cleaned, decrypted, OCR-ed, edited, annotated, translated, sourced, ...
);




CREATE TABLE author4metarecord(
	metarecord INTEGER REFERENCES metadata_record,
	party INTEGER REFERENCES party,
	purpose INTEGER NOT NULL REFERENCES purpose,			
						-- author, primary author, subsidiary author, sponsor, editor, conference editor, series editor, investigator, translator, ..., 
						-- publisher, sponsor, conference organizer, *corporate author*, ...

	PRIMARY KEY(metarecord, party, purpose)
)  WITHOUT ROWID;



CREATE TABLE affiliation4author(
	party INTEGER REFERENCES party,
	affiliation INTEGER REFERENCES party,
	purpose INTEGER NOT NULL REFERENCES purpose,			
 						-- employer, sponsor, ..., colleague, project lead, ...

	PRIMARY KEY(party, affiliation, purpose)
)  WITHOUT ROWID;



CREATE TABLE url4metarecord(
	id INTEGER PRIMARY KEY,
	metarecord INTEGER REFERENCES metadata_record,
	url TEXT,
	last_checked REAL,				-- timestamp when last accessed
	function TEXT					-- original, preprint, ...
);



CREATE TABLE cited_ref4metarecord(
	metarecord INTEGER REFERENCES metadata_record,
	cited_metarecord INTEGER REFERENCES metadata_record,
	function TEXT,					-- citation, ...

	PRIMARY KEY(metarecord, cited_metarecord, function)
) WITHOUT ROWID;



CREATE TABLE grouping4metarecord(
	child_metarecord INTEGER REFERENCES metadata_record,		-- child element, e.g. paper (a.k.a. article)
	group_metarecord INTEGER REFERENCES metadata_record,		-- parent element, e.g. volume / book / conference syllabus / book series
	child_function TEXT,			-- article, extract / excerpt, TOC, promo material, editorial, ...
	group_function TEXT,			-- volume, book / magazine, conference syllabus, collection, ...

	PRIMARY KEY(child_metarecord, group_metarecord, child_function, group_function)
) WITHOUT ROWID;



--
-- used to track edit history and publication history both:
--
CREATE TABLE revision4metarecord(
	revision INTEGER PRIMARY KEY,
	root INTEGER NOT NULL REFERENCES metadata_root,					-- just a helper so we can filter extract the revision history subgraph for the given document

	current_metarecord INTEGER NOT NULL REFERENCES metadata_record,
	prev_A_metarecord INTEGER REFERENCES metadata_record,
	prev_B_metarecord INTEGER REFERENCES metadata_record,	-- NULL, unless this is a merge revision (merging A + B -> current)

	mark REAL,						 -- timestamp when observed / committed / TBD...
	author INTEGER NOT NULL REFERENCES party, 
	                                 -- the author of this revision record. MAY be an automaton, e.q. "Qiqqa Heuristics Robot #NNN", 
	                                 -- where #NNN would identify the actual machine where this was inserted. 
									 --
									 -- Why a direct 1:1 link and not a simple entry in the author4metarecord table? Well, because:
									 -- 1. there can only be ONE author of a "commit", i.e. a database edit like this. This is a technical action and has nothing to do 
									 --    with the authors who will be mentioned in metadata or are otherwise related to the document by having them added to that relation table
									 --    by the owner(s) of this database.
									 -- 2. this author in particular is NOT NULL restricted as *every* edit to our revision set should be tracable to an editor, 
									 --    whether man or machine.
	purpose INTEGER NOT NULL REFERENCES purpose,			
	                                 -- duplicate, (re)published, original, cleaned, decrypted, OCR-ed, edited, annotated, translated, sourced, ...
	                                 --
									 -- 'sourced' means this record revision carries information which has been imported from "external sources" (which can be *anything*).
									 -- This is our way to collect and store public bibTeX records and such, as this way allows us to freely remix such "foreign" content
									 -- with local user- or automation-driven "edits", which would be stored in additional metarecords. This also ensures we'll always
									 -- be dig up where a particular bit of (meta)data originated from.
	notes TEXT                       -- like a "git commit -m" message. Optional.
);


PRAGMA table_list;

PRAGMA schema.user_version = 1;
PRAGMA schema.user_version;

.dbinfo
.show
.databases
.tables
.quit

