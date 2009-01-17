AM_CXXFLAGS = -I$(top_srcdir)/ -I$(top_srcdir)/Lua/src

bin_PROGRAMS = Epiar

Epiar_SOURCES = main.cpp AI/ai.cpp Input/input.cpp Engine/alliances.cpp \
                Engine/engines.cpp Engine/hud.cpp Engine/models.cpp \
                Engine/simulation.cpp Engine/starfield.cpp \
                Sprites/planets.cpp Sprites/player.cpp Sprites/ship.cpp \
                Sprites/sprite.cpp Sprites/spritelist.cpp \
                Utilities/camera.cpp Utilities/cmath.cpp Utilities/coordinate.cpp \
                Utilities/hashtbl.cpp Utilities/log.cpp \
                Utilities/timer.cpp Utilities/trig.cpp Utilities/vector.cpp \
                UI/ui_widget.cpp UI/ui_window.cpp UI/ui.cpp Graphics/afont/afont_base.cpp \
                Graphics/afont/afont_gl.cpp Graphics/animation.cpp Graphics/font.cpp Graphics/image.cpp \
                Graphics/video.cpp Utilities/archive.cpp Utilities/lua.cpp Utilities/xml.cpp \
                Utilities/file.cpp UI/ui_button.cpp UI/ui_label.cpp Utilities/debug.cpp

Epiar_LDADD = Lua/src/liblua.a

SUBDIRS=Lua
