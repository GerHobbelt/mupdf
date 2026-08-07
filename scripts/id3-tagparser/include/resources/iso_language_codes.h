
#pragma once

// from \thirdparty\owemdjee\id3-tagparser\cmake\scripts\generate_iso_language_codes.cmake

static const std::unordered_map<std::string, std::string> languageNames_iso_639_2_b = {
/*
file(READ "${LANGUAGE_FILE}" LANGUAGE_JSON)
string(JSON LANGUAGE_COUNT LENGTH "${LANGUAGE_JSON}" "639-2")
message(STATUS "Found ${LANGUAGE_COUNT} ISO-639-2 language codes")
math(EXPR LANGUAGE_COUNT "${LANGUAGE_COUNT} - 1")
foreach (LANGUAGE_INDEX RANGE "${LANGUAGE_COUNT}")
    string(JSON LANGUAGE_ENTRY GET "${LANGUAGE_JSON}" "639-2" "${LANGUAGE_INDEX}")
    string(JSON LANGUAGE_ALPHA_2 ERROR_VARIABLE ERROR GET "${LANGUAGE_ENTRY}" "alpha_2")
    string(JSON LANGUAGE_ALPHA_3 ERROR_VARIABLE ERROR GET "${LANGUAGE_ENTRY}" "alpha_3")
    string(JSON LANGUAGE_BIBLIOGRAPHIC ERROR_VARIABLE ERROR GET "${LANGUAGE_ENTRY}" "bibliographic")
    string(JSON LANGUAGE_NAME ERROR_VARIABLE ERROR GET "${LANGUAGE_ENTRY}" "name")
    if (NOT LANGUAGE_BIBLIOGRAPHIC)
        set(LANGUAGE_BIBLIOGRAPHIC "${LANGUAGE_ALPHA_3}")
    endif ()
    set(OUTPUT "${OUTPUT}    {\"${LANGUAGE_BIBLIOGRAPHIC}\", \"${LANGUAGE_NAME}\"},\n")
endforeach ()
*/
};
