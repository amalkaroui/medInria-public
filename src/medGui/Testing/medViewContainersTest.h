// /////////////////////////////////////////////////////////////////
// Generated by dtkTestGenerator
// /////////////////////////////////////////////////////////////////

#pragma once

#include <dtkCore/dtkTest.h>

class medViewContainersTestObject : public QObject
{
    Q_OBJECT

public:
             medViewContainersTestObject(void);
    virtual ~medViewContainersTestObject(void);


private slots:
    /*
     * initTestCase() is called before the
     * execution of all the test cases.
     * If it fails, no test is executed.
     */
    void initTestCase();

    /*
     * init() is called before each test.
     * If it fails, the following
     * test is not executed.
     */
    void init();

    /*
     * cleanup() is called after each test.
     */
    void cleanup();

    /*
     * cleanupTestCase() is called
     * after all test have been executed.
     */
    void cleanupTestCase();

private slots:

    // every function here is a test, unless they end with "_data"
    // in this case "testFoo_data()" will prepare some data
    // to run "testFoo()" different times
    // with different input values
    // void testFoo_data();
    void testSingle();
    void testMulti();
    void testCustom();
};


