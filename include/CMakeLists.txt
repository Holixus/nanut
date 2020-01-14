option(INSTALL_NANO_UT_HEADERS "Install libnanut headers." ON)

if(INSTALL_NANO_UT_HEADERS)

	file(GLOB headers "nano/ut*.h")

	install(FILES ${headers}
		DESTINATION include/nano
		PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

	CONFIGURE_FILE(nano/ut.h.in nano/ut.h @ONLY)

endif(INSTALL_NANO_UT_HEADERS)
