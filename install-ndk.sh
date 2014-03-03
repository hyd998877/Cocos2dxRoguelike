#!/bin/sh

# Install NDK
wget "http://dl.google.com/android/ndk/android-ndk-r${ndk_version}-linux-x86_64.tar.bz2"
tar xvjf "android-ndk-r${ndk_version}-linux-x86_64.tar.bz2"
rm "android-ndk-r${ndk_version}-linux-x86_64.tar.bz2"

# Set environment variables
export ANDROID_NDK_HOME=${PWD}/android-ndk-r${ndk_version}

# Write environment variables setup to $profile
echo "export ANDROID_NDK_HOME=\"${ANDROID_NDK_HOME}\"" | sudo tee -a $profile

# Print $profile
echo -e "$profile updated to:\n\n$(cat $profile)"
echo "export ANDROID_NDK_VERSION=\"${ndk_version}\"" | sudo tee -a $profile