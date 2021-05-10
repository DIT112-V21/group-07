package com.example.pathfinder;


import com.example.pathfinder.Activities.DriverLogin;

import org.junit.Before;
import org.junit.Test;

import java.util.HashMap;

import static com.google.common.truth.Truth.assertThat;

public class DriverLoginTest {

    /** create a new instance of the driverLogin class */
    DriverLogin driverLogin;


    /**
     * Setup the test class for running the different cases:
     *  - initialise the driverLogin class
     *  - initialise the Map containing the usernames and passwords
     *  - add a username and a password associated
     */
    @Before
    public void setup(){
        driverLogin = new DriverLogin();
        driverLogin.setDriversProfiles(new HashMap<>());
        driverLogin.getDriversProfiles().put("Alex", "1234");
    }


    /**
     * Tests if verifying that a username is registered in the map works correctly
     * Associated method: isRegisteredUsername();
     */
    @Test
    public void isRegisteredUsernameTest_returnsTrue(){
        assertThat(driverLogin.isRegisteredUsername("Alex")).isTrue();
    }

    /**
     * Tests if verifying that a username is not registered in the map works correctly
     * Associated method: isRegisteredUsername();
     */
    @Test
    public void isUnregisteredUsernameTest_returnsFalse(){
        assertThat(driverLogin.isRegisteredUsername("George")).isFalse();
    }

    /**
     * Tests if verifying that a given password is matching the one of a registered user works correctly.
     * Associated method: isMatchingPassword();
     */
    @Test
    public void isMatchingPasswordTest_returnTrue(){
        assertThat(driverLogin.isMatchingPassword("Alex", "1234")).isTrue();
        }

    /**
     * Tests if verifying that a given password is not matching the one of a registered user works correctly.
     * Associated method: isMatchingPassword();
     */
    @Test
    public void isNotMatchingPasswordTest_returnFalse(){
        assertThat(driverLogin.isMatchingPassword("Alex", "123")).isFalse();
    }
}
