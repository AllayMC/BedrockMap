add_rules('mode.release', 'mode.debug')

add_repositories('repo third-party')

add_requires('gtest         1.15.2')
add_requires('nlohmann_json 3.11.3')
add_requires('stb           2024.6.1')

add_requires('pmmp-leveldb  2021.10.9')

target('BedrockMap')
    add_rules('qt.widgetapp')
    add_files('src/**.cpp') -- source files
    add_files('src/**.ui')  -- qt widgets
    add_files('src/**.h')   -- qt meta header

    set_warnings('all')
    set_languages('cxx23', 'c23')
    
    if is_plat('windows') then
        add_files('assets/icon.rc')
    end

    add_frameworks(
        'QtWidgets',
        'QtConcurrent',

        -- TODO: fully migrate to Qt6
        'QtCore5Compat'
    )

    add_packages(
        'nlohmann_json',
        'stb'
    )
    add_packages('pmmp-leveldb')

    add_includedirs(
        'src'
    )
