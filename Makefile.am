AM_CXXFLAGS = -I$(top_srcdir)/Source/ -I$(top_srcdir)/Source/Lua/src

bin_PROGRAMS = Epiar

Epiar_SOURCES = Source/main.cpp Source/AI/ai.cpp Source/AI/ai_lua.cpp Source/Input/input.cpp Source/Engine/alliances.cpp \
                Source/Engine/engines.cpp Source/Engine/hud.cpp Source/Engine/models.cpp Source/Engine/weaponSystem.cpp \
                Source/Engine/simulation.cpp Source/Engine/starfield.cpp Source/Engine/weapon.cpp Source/Engine/weapons.cpp \
                Source/Sprites/planets.cpp Source/Sprites/player.cpp Source/Sprites/ship.cpp \
                Source/Sprites/sprite.cpp Source/Sprites/spritemanager.cpp Source/Sprites/projectile.cpp \
                Source/Utilities/camera.cpp Source/Utilities/cmath.cpp Source/Utilities/coordinate.cpp \
                Source/Utilities/hashtbl.cpp Source/Utilities/log.cpp \
                Source/Utilities/timer.cpp Source/Utilities/trig.cpp Source/Utilities/vector.cpp \
                Source/UI/ui_widget.cpp Source/UI/ui_window.cpp Source/UI/ui_lua.cpp Source/UI/ui.cpp Source/Graphics/afont/afont_base.cpp \
                Source/Graphics/afont/afont_gl.cpp Source/Graphics/animation.cpp Source/Graphics/font.cpp Source/Graphics/image.cpp \
                Source/Graphics/video.cpp Source/Utilities/archive.cpp Source/Utilities/lua.cpp Source/Utilities/xml.cpp \
                Source/Utilities/file.cpp Source/UI/ui_button.cpp Source/UI/ui_label.cpp Source/UI/ui_picture.cpp Source/Utilities/debug.cpp \
                Source/Engine/console.cpp Source/Engine/quadrant.cpp Source/UI/ui_textbox.cpp Source/Utilities/quadtree.cpp

Epiar_LDADD = Source/Lua/src/liblua.a

SUBDIRS=Source/Lua
