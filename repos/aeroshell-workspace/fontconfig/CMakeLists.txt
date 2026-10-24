set(conf_file 95-segoeui-hinting.conf)
set(conf_dest ${CMAKE_INSTALL_PREFIX}/share/fontconfig/conf.avail)
set(conf_link ${KDE_INSTALL_SYSCONFDIR}/fonts/conf.d)

install(FILES ${CMAKE_SOURCE_DIR}/fontconfig/${conf_file} DESTINATION ${conf_dest})
install(FILES ${CMAKE_SOURCE_DIR}/fontconfig/${conf_file} DESTINATION ${conf_link}) # Hack to ensure the file is listed in install_manifest.txt

install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E rm \
    $ENV{DESTDIR}${conf_link}/${conf_file} \
    )"
)
install(CODE "execute_process( \
    COMMAND ${CMAKE_COMMAND} -E create_symlink \
    ${conf_dest}/${conf_file} \
    $ENV{DESTDIR}${conf_link}/${conf_file} \
    )"
)
