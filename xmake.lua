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
    set_optimize('fastest')
    set_languages('cxx23', 'c23')

    if is_plat('windows') then
        add_files('assets/icon.rc')
    end

    add_frameworks(
        'QtWidgets',
        'QtConcurrent'
    )

    add_packages(
        'nlohmann_json',
        'stb'
    )
    add_packages('pmmp-leveldb')

    add_includedirs(
        'src'
    )

    -- autogen qrc
    add_files('assets/autogen.qrc')

    on_load(function (target)
        -- placeholder to prevent configure failure.
        io.writefile('assets/autogen.qrc', '<!--This file will be automatically generated before building.-->\n<RCC></RCC>')
    end)
    before_build(function (target)
        local assets_dir = 'assets/'
        local qrc = '<RCC>\n\t<qresource prefix="/">\n'
        local qrc_including_assets = {
            'ui/**',
            'mc/**'
        }
        for _, prefix in ipairs(qrc_including_assets) do
            for _, path in ipairs(os.files(assets_dir .. prefix)) do
                qrc = qrc .. ('\t\t<file>%s</file>\n'):format(path:sub(assets_dir:len() + 1))
            end
        end
        qrc = qrc .. '\t</qresource>\n</RCC>'
        old_qrc = io.readfile(assets_dir .. 'autogen.qrc')
        if qrc ~= old_qrc then
            io.writefile(assets_dir .. 'autogen.qrc', qrc)
        end
    end)
