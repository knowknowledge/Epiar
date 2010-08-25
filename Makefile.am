AM_CXXFLAGS = -I$(top_srcdir)/Source/ -I$(top_srcdir)/Source/Lua/src

bin_PROGRAMS = Epiar

Epiar_SOURCES = Source/main.cpp \
                Source/AI/ai.cpp \
                Source/AI/ai_lua.cpp \
                Source/Audio/audio.cpp \
                Source/Audio/audio_lua.cpp \
                Source/Audio/music.cpp \
                Source/Audio/sound.cpp \
                Source/Engine/alliances.cpp \
                Source/Engine/commodities.cpp \
                Source/Engine/console.cpp \
                Source/Engine/engines.cpp \
                Source/Engine/hud.cpp \
                Source/Engine/models.cpp \
                Source/Engine/outfit.cpp \
                Source/Engine/quadrant.cpp \
                Source/Engine/simulation.cpp \
                Source/Engine/starfield.cpp \
                Source/Engine/technologies.cpp \
                Source/Engine/weapon.cpp \
                Source/Engine/weapons.cpp \
                Source/Graphics/animation.cpp \
                Source/Graphics/font.cpp \
                Source/Graphics/image.cpp \
                Source/Graphics/video.cpp \
                Source/Input/input.cpp \
                Source/Sprites/effects.cpp \
                Source/Sprites/gate.cpp \
                Source/Sprites/planets.cpp \
                Source/Sprites/player.cpp \
                Source/Sprites/projectile.cpp \
                Source/Sprites/ship.cpp \
                Source/Sprites/sprite.cpp \
                Source/Sprites/spritemanager.cpp \
                Source/UI/ui.cpp \
                Source/UI/ui_button.cpp \
                Source/UI/ui_checkbox.cpp \
                Source/UI/ui_container.cpp \
                Source/UI/ui_label.cpp \
                Source/UI/ui_lua.cpp \
                Source/UI/ui_picture.cpp \
                Source/UI/ui_scrollbar.cpp \
                Source/UI/ui_slider.cpp \
                Source/UI/ui_tabs.cpp \
                Source/UI/ui_textbox.cpp \
                Source/UI/ui_widget.cpp \
                Source/UI/ui_window.cpp \
                Source/UI/ui_frame.cpp \
                Source/Utilities/argparser.cpp \
                Source/Utilities/camera.cpp \
                Source/Utilities/cmath.cpp \
                Source/Utilities/components.cpp \
                Source/Utilities/coordinate.cpp \
                Source/Utilities/debug.cpp \
                Source/Utilities/file.cpp \
                Source/Utilities/filesystem.cpp \
                Source/Utilities/hashtbl.cpp \
                Source/Utilities/log.cpp \
                Source/Utilities/lua.cpp \
                Source/Utilities/quadtree.cpp \
                Source/Utilities/resource.cpp \
                Source/Utilities/timer.cpp \
                Source/Utilities/trig.cpp \
                Source/Utilities/vector.cpp \
                Source/Utilities/xml.cpp

Epiar_LDADD = Source/Lua/src/liblua.a

SUBDIRS=Source/Lua
