install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION /usr/local/remote-client
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/config.ini 
    DESTINATION /usr/local/remote-client
)
