#!/bin/bash

if [ $# -ne 2 ]
  then
	echo "Incorrect arguments"
	echo "Please pass path of RPM as the first argument"
	echo "Please pass path of Tizen SDK as the second argument"
	echo "Example usage:"
	echo "    ./update_sdk_rootstrap.sh ~/GBS-Kiran/liboauth2/local/repos/tizen/i586/RPMS/ ~/Tizen-SDK-2.3/tizen-sdk/"
else
	if [ -z "$1" ];
	  then
		echo "Please pass the correct path of RPM as first argument"
	else
		rpm_path=$1
		echo $rpm_path
		cd $rpm_path
		sudo rm -rf usr
		if [[ $rpm_path == *"armv7l"* ]]
		then
			echo "Device"
			if [ -a oauth2-0.0.1-1.armv7l.rpm ]
			  then
				sudo rpm2cpio oauth2-0.0.1-1.armv7l.rpm | cpio -idmv
			else
				echo "oauth2-0.0.1-1.armv7l.rpm file does not exist"
			fi

			if [ -a oauth2-devel-0.0.1-1.armv7l.rpm ]
			  then
				sudo rpm2cpio oauth2-devel-0.0.1-1.armv7l.rpm | cpio -idmv
			else
				echo "oauth2-devel-0.0.1-1.armv7l.rpm file does not exist"
			fi
		else
			echo "Emulator"
			if [ -a oauth2-0.0.1-1.i586.rpm ]
			  then
				sudo rpm2cpio oauth2-0.0.1-1.i586.rpm | cpio -idmv
			else
				echo "oauth2-0.0.1-1.i586.rpm file does not exist"
			fi

			if [ -a oauth2-devel-0.0.1-1.i586.rpm ]
			  then
				sudo rpm2cpio oauth2-devel-0.0.1-1.i586.rpm | cpio -idmv
			else
				echo "oauth2-devel-0.0.1-1.i586.rpm file does not exist"
			fi
		fi
	fi

	if [ -z "$2" ];
	  then
		echo "Please pass the correct path of RPM as second argument"
	else
		sdk_path=$2
		if [[ $rpm_path == *"armv7l"* ]]
		  then
			if [[ ${sdk_path: -1} == "/" ]]
			then
				sdk_path_lib="${sdk_path}platforms/mobile-2.3/rootstraps/mobile-2.3-device.core/usr/lib/"
				sdk_path_inc="${sdk_path}platforms/mobile-2.3/rootstraps/mobile-2.3-device.core/usr/include/"
			else
				sdk_path_lib="${sdk_path}/platforms/mobile-2.3/rootstraps/mobile-2.3-device.core/usr/lib/"
				sdk_path_inc="${sdk_path}/platforms/mobile-2.3/rootstraps/mobile-2.3-device.core/usr/include/"
			fi
		else
			if [[ ${sdk_path: -1} == "/" ]]
			then
				sdk_path_lib="${sdk_path}platforms/mobile-2.3/rootstraps/mobile-2.3-emulator.core/usr/lib/"
				sdk_path_inc="${sdk_path}platforms/mobile-2.3/rootstraps/mobile-2.3-emulator.core/usr/include/"
			else
				sdk_path_lib="${sdk_path}/platforms/mobile-2.3/rootstraps/mobile-2.3-emulator.core/usr/lib/"
				sdk_path_inc="${sdk_path}/platforms/mobile-2.3/rootstraps/mobile-2.3-emulator.core/usr/include/"
			fi
		fi

		sudo cp usr/lib/liboauth2.so* $sdk_path_lib
		sudo cp usr/include/oauth2* $sdk_path_inc
		sudo rm -rf usr
		echo "SO and Headers copied"
	fi
fi
