SET ( CUSTOMISED_FILES_DIR ${CMAKE_SOURCE_DIR}/user_customisable/${CFG_ARM_BOARD_TYPE} )

SET ( FILE_APP_C app.c )
SET ( FILE_APP_GPIO_C app-gpio.c )
SET ( CHECK_FILENAMES ${FILE_APP_C} ${FILE_APP_GPIO_C} )

SET ( ORIG_FILE_APP_C_PATH ${DEMO_SAPL_DIR}/${FILE_APP_C} )

SET ( FILE_APP_C_SOURCE_PATH ${PYSODBUILDER_OUTPUT_DIR}/app.c )
SET ( FILE_APP_GPIO_C_SOURCE_PATH ${TARGET_DIR}/app-gpio.c )

SET ( CUSTOMISED_FILE_APP_C ${CUSTOMISED_FILES_DIR}/app.c )
SET ( CUSTOMISED_FILE_APP_GPIO_C ${CUSTOMISED_FILES_DIR}/app-gpio.c )
SET ( CUSTOMISED_TIMESTAMP ${CUSTOMISED_FILES_BUILD_DIR}/customtimestamp )

IF ( USE_CUSTOMISED_FILES )
    #copy files to userfolder if they do not exist

    IF ( NOT EXISTS  ${CUSTOMISED_FILES_DIR}/${FILE_APP_C} )
    #copy app.c from generated files
        IF ( EXISTS ${FILE_APP_C_SOURCE_PATH} )
            FILE ( COPY ${FILE_APP_C_SOURCE_PATH} DESTINATION ${CUSTOMISED_FILES_DIR} )
        ENDIF ()
    ENDIF()

    IF ( NOT EXISTS ${CUSTOMISED_FILES_DIR}/${FILE_APP_GPIO_C} )
    #copy app-gpio.c from app/target/<Processor>
        IF ( EXISTS ${FILE_APP_GPIO_C_SOURCE_PATH} )
            FILE ( COPY ${FILE_APP_GPIO_C_SOURCE_PATH} DESTINATION ${CUSTOMISED_FILES_DIR} )
        ENDIF ()
    ENDIF()

    # copy files from user folder to build folder for compilation
    SET ( FILES_TO_COPY
        ${CUSTOMISED_FILE_APP_C}
        ${CUSTOMISED_FILE_APP_GPIO_C}
        )

    FILE ( GLOB_RECURSE BUILD_DIR_FILES ${CUSTOMISED_FILES_BUILD_DIR}/* )
    LIST ( LENGTH BUILD_DIR_FILES LISTLENGTH )

   IF ( NOT LISTLENGTH )
        FILE ( COPY ${FILES_TO_COPY} DESTINATION ${CUSTOMISED_FILES_BUILD_DIR} )
    ELSE ()
        #set custom command to copy at compile time
        #this is needed when the user customised code has changed,
        #and no further cmake generate step has been carried out.
        ADD_CUSTOM_COMMAND (
            OUTPUT ${CUSTOMISED_TIMESTAMP}
            COMMAND ${CMAKE_COMMAND} -E touch ${CUSTOMISED_TIMESTAMP}
        )

        ADD_CUSTOM_TARGET (
            use_customised_files
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CUSTOMISED_FILES_DIR} ${CUSTOMISED_FILES_BUILD_DIR}
            COMMENT "Copying customised files to build directory"
            DEPENDS ${FILES_TO_COPY} ${CUSTOMISED_TIMESTAMP}
        )

        SET ( ADD_CUSTOM_FILES_DEP_TO_APP_TARGET TRUE )

    ENDIF()

ELSE ()
    IF ( PYSODBUILDER_ENABLE )
            #copy app.c from pysodbuilder, app-gpio.c from original source
            ADD_CUSTOM_TARGET (
                use_customised_files
                COMMAND ${CMAKE_COMMAND} -E copy
                ${FILE_APP_GPIO_C_SOURCE_PATH} ${CUSTOMISED_FILES_BUILD_DIR}
                COMMAND ${CMAKE_COMMAND} -E copy
                ${FILE_APP_C_SOURCE_PATH} ${CUSTOMISED_FILES_BUILD_DIR}
                COMMENT "Copying original app-gpio.c and generated app.c to build directory"
                DEPENDS ${FILE_APP_GPIO_C_SOURCE_PATH} ${FILE_APP_C_SOURCE_PATH}
            )

        SET ( ADD_CUSTOM_FILES_DEP_TO_APP_TARGET TRUE )

    ELSE()
        #copy files from original source directory, no copy at compile time needed
        #since the files should not change
        FILE ( COPY ${ORIG_FILE_APP_C_PATH} DESTINATION ${CUSTOMISED_FILES_BUILD_DIR} )
        FILE ( COPY ${FILE_APP_GPIO_C_SOURCE_PATH} DESTINATION ${CUSTOMISED_FILES_BUILD_DIR} )
        FILE ( GLOB_RECURSE BUILD_DIR_FILES ${CUSTOMISED_FILES_BUILD_DIR}/* )

        FOREACH (_file IN ITEMS ${BUILD_DIR_FILES})
            EXECUTE_PROCESS ( COMMAND ${CMAKE_COMMAND} -E touch ${_file} )
        ENDFOREACH()
    ENDIF()
ENDIF()

IF ( ADD_CUSTOM_FILES_DEP_TO_APP_TARGET AND DEMO_SAFETY_APP_TARGET_NAME )
    ADD_DEPENDENCIES( ${DEMO_SAFETY_APP_TARGET_NAME} use_customised_files )
ENDIF( ADD_CUSTOM_FILES_DEP_TO_APP_TARGET AND DEMO_SAFETY_APP_TARGET_NAME )
