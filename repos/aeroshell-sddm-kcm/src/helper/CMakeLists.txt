add_executable(kcmaeroshellsddmhelper helper.h helper.cpp)

target_link_libraries(kcmaeroshellsddmhelper
    PRIVATE
        KF6::AuthCore
        KF6::ConfigCore
)
install(TARGETS kcmaeroshellsddmhelper DESTINATION ${KAUTH_HELPER_INSTALL_DIR})

kauth_install_helper_files(kcmaeroshellsddmhelper aeroshell.kcmaeroshellsddm root)
kauth_install_actions(aeroshell.kcmaeroshellsddm kcm_aeroshellsddm.actions)
