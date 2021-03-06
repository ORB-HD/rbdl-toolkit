#!/bin/bash
#
# This skript creates an appimage which can build rbdl-toolkit plugins.
#
# To do this we need to do the following things:
# 1. Create /build in appimage dir
# 2. Copy Toolkit Src and builddir into /build
# 3. Copy Qt-Libraries into /build
# 4. Add skipt which will perform all the build steps needed

# Check that we are specifing a Qt Library dir that only contains the stuff needed for qt
QT_PATH="${CUSTOM_QT_PATH}"
if [ "$QT_PATH" == "" ]; then
	echo "This script expects a custom installed Qt, because filtering the system libraries for the relevant Qt libraries and copying them is to much work! Aborting …"
	exit 1
fi


APPDIR=toolkit-pluginbuilder.AppDir

mkdir -p toolkit-pluginbuilder.AppDir

BUILDFILESPATH=$APPDIR/build

mkdir -p $BUILDFILESPATH

# Make sure project is built
make

if (( $? )); then
	exit 1
fi

TOOLKIT_SRC="${RBDL-Toolkit_SOURCE_DIR}"
TOOLKIT_BUILD="${RBDL-Toolkit_BINARY_DIR}"

echo "Copying Toolkit source to AppDir"
rsync -r --filter=':- .gitignore' $TOOLKIT_SRC $BUILDFILESPATH
if (( $? )); then
	exit 1
fi

echo "Copying Toolkit build files to AppDir"
rsync -r --exclude='*.AppDir' --exclude='*.AppImage' $TOOLKIT_BUILD/ $BUILDFILESPATH/toolkit_build
if (( $? )); then
	exit 1
fi

TOOLKIT_BUILD=$BUILDFILESPATH/toolkit_build
TOOLKIT_SRC=$BUILDFILESPATH/$(basename $TOOLKIT_SRC)

echo "Copying Qt libraries to AppDir"
rsync -rlp $QT_PATH/ $BUILDFILESPATH/qt
if (( $? )); then
	exit 1
fi
#this is a hacky fix, but works for now
sed -i "s/\/usr\/lib\/x86_64-linux-gnu\/libEGL.so/EGL/g" $BUILDFILESPATH/qt/lib/cmake/Qt5Gui/Qt5GuiConfigExtras.cmake
sed -i "s/\/usr\/lib\/x86_64-linux-gnu\/libGL.so/GL/g" $BUILDFILESPATH/qt/lib/cmake/Qt5Gui/Qt5GuiConfigExtras.cmake

mkdir -p $APPDIR/usr/bin
cp $TOOLKIT_SRC/extra/mk_plugin.sh $APPDIR/usr/bin
if (( $? )); then
	exit 1
fi

echo "Deploy buildtools and icon"
linuxdeploy --appdir=$APPDIR -i ../extra/rbdl-toolkit.png -e ${CMAKE_COMMAND} -e ${CMAKE_CPACK_COMMAND} -e ${CMAKE_CTEST_COMMAND} -e ${CMAKE_CXX_COMPILER} -e ${CMAKE_MAKE_PROGRAM} -d ../extra/buildplugin.desktop
if (( $? )); then
	exit 1
fi

#copy cmake root
rsync -rp ${CMAKE_ROOT} $APPDIR/usr/share
if (( $? )); then
	exit 1
fi


ln -sf usr/share/icons/hicolor/256x256/apps/rbdl-toolkit.png $APPDIR
if (( $? )); then
	exit 1
fi


#create AppImage
appimagetool $APPDIR

