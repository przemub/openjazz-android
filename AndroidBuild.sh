#!/bin/sh
# This *really* complicated script prepares OpenJazz for packaging.
#
# Part of OpenJazz for Android
# Copyright (c) 2013 Przemysław Buczkowski <przemub@przemub.pl>
#
# OpenJazz is distributed under the terms of
# the GNU General Public License, version 2.0
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


../setEnvironment.sh sh -c "make"

mv -f OpenJazz libapplication.so
