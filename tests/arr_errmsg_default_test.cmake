execute_process(
    COMMAND "${PROGRAM}" "${CASE}"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE expected
    ERROR_VARIABLE actual
)

if(NOT "${result}" STREQUAL "0")
    message(FATAL_ERROR "${CASE}: unexpected exit ${result}\n${actual}")
endif()

if(NOT "${actual}" STREQUAL "${expected}")
    message(FATAL_ERROR "${CASE}: stderr differs\nExpected: ${expected}\nActual: ${actual}")
endif()
