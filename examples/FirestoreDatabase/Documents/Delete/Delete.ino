/**
 * SYNTAXES:
 *
 * Docs.deleteDoc(<AsyncClient>, <Firestore::Parent>, <documentPath>, <Precondition>);
 * Docs.deleteDoc(<AsyncClient>, <Firestore::Parent>, <documentPath>, <Precondition>, <AsyncResult>);
 * Docs.deleteDoc(<AsyncClient>, <Firestore::Parent>, <documentPath>, <Precondition>, <AsyncResultCallback>, <uid>);
 *
 * The <Firestore::Parent> is the Firestore::Parent object included project Id and database Id in its constructor.
 * The Firebase project Id should be only the name without the firebaseio.com.
 * The Firestore database id should be (default) or empty "".
 *
 * The <Precondition> is the Precondition object for an optional precondition on the document.
 * The request will fail if this is set and not met by the target document.
 *
 * There are the member functions exists and updateTime for creating the union field exists and updateTime respectively.
 *
 * The exists option, when set to true, the target document must exist. When set to false, the target document must not exist.
 * The updateTime (timestamp) option, when set, the target document must exist and have been last updated at that time.
 * A timestamp is in RFC3339 UTC "Zulu" format, with nanosecond resolution and up to nine fractional digits.
 * Examples: "2014-10-02T15:01:23Z" and "2014-10-02T15:01:23.045123456Z".
 *
 * The async functions required AsyncResult or AsyncResultCallback function that keeping the result.
 *
 * The uid is user specified UID of async result (optional) which used as async task identifier.
 *
 * The uid can later get from AsyncResult object of AsyncResultCallback function via aResult.uid().
 * 
 * The complete usage guidelines, please visit https://github.com/mobizt/FirebaseClient
 */

#include <Arduino.h>
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#elif __has_include(<WiFiNINA.h>)
#include <WiFiNINA.h>
#elif __has_include(<WiFi101.h>)
#include <WiFi101.h>
#elif __has_include(<WiFiS3.h>)
#include <WiFiS3.h>
#endif

#include <FirebaseClient.h>

#define WIFI_SSID "WIFI_AP"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// The API key can be obtained from Firebase console > Project Overview > Project settings.
#define API_KEY "Web_API_KEY"

// User Email and password that already registerd or added in your project.
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"
#define DATABASE_URL "URL"

#define FIREBASE_PROJECT_ID "PROJECT_ID"

void asyncCB(AsyncResult &aResult);

void printResult(AsyncResult &aResult);

DefaultNetwork network; // initilize with boolean parameter to enable/disable network reconnection

UserAuth user_auth(API_KEY, USER_EMAIL, USER_PASSWORD);

FirebaseApp app;

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
#include <WiFiClientSecure.h>
WiFiClientSecure ssl_client;
#elif defined(ARDUINO_ARCH_SAMD)
#include <WiFiSSLClient.h>
WiFiSSLClient ssl_client;
#endif

// In case the keyword AsyncClient using in this example was ambigous and used by other library, you can change
// it with other name with keyword "using" or use the class name AsyncClientClass directly.

using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client, getNetwork(network));

Firestore::Documents Docs;

AsyncResult aResult_no_callback;

int counter = 0;

unsigned long dataMillis = 0;

bool taskCompleted = false;

void setup()
{

    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Connecting to Wi-Fi");
    unsigned long ms = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

    Serial.println("Initializing app...");

#if defined(ESP32) || defined(ESP8266) || defined(PICO_RP2040)
    ssl_client.setInsecure();
#if defined(ESP8266)
    ssl_client.setBufferSizes(4096, 1024);
#endif
#endif
    app.setCallback(asyncCB);

    initializeApp(aClient, app, getAuth(user_auth));

    // Waits for app to be authenticated.
    // For asynchronous operation, this blocking wait can be ignored by calling app.loop() in loop().
    ms = millis();
    while (app.isInitialized() && !app.ready() && millis() - ms < 120 * 1000)
        ;

    app.getApp<Firestore::Documents>(Docs);
}

void loop()
{
    // This function is required for handling async operations and maintaining the authentication tasks.
    app.loop();

    // This required when different AsyncClients than used in FirebaseApp assigned to the Firestore functions.
    Docs.loop();

    // To get anyc result without callback
    // printResult(aResult_no_callback);

    if (app.ready() && (millis() - dataMillis > 60000 || dataMillis == 0))
    {
        dataMillis = millis();

        if (!taskCompleted)
        {
            taskCompleted = true;

            // aa is the collection id, bb is the document id in collection aa.
            String documentPath = "aa/bb";

            Serial.println("Create a document... ");

            Document<Values::Value> doc("myDouble", Values::Value(Values::DoubleValue(123.456)));

            Docs.createDocument(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, DocumentMask(), doc, asyncCB);

            Serial.println("Delete a document... ");

            Docs.deleteDoc(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, Precondition() /* Precondition (currentocument) */, asyncCB);

            // To assign UID for async result
            // Docs.deleteDoc(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, Precondition() /* Precondition (currentocument) */, asyncCB, "myUID");

            // To get anyc result without callback
            // Docs.deleteDoc(aClient, Firestore::Parent(FIREBASE_PROJECT_ID), documentPath, Precondition() /* Precondition (currentocument) */, aResult_no_callback);
        }
    }
}

void asyncCB(AsyncResult &aResult)
{

    // To get the UID (string) from async result
    // aResult.uid();

    printResult(aResult);
}

void printResult(AsyncResult &aResult)
{
    if (aResult.appEvent().code() > 0)
    {
        Firebase.printf("Event msg: %s, code: %d\n", aResult.appEvent().message().c_str(), aResult.appEvent().code());
    }

    if (aResult.isDebug())
    {
        Firebase.printf("Debug msg: %s\n", aResult.debug().c_str());
    }

    if (aResult.isError())
    {
        Firebase.printf("Error msg: %s, code: %d\n", aResult.error().message().c_str(), aResult.error().code());
    }

    if (aResult.available())
    {
        Firebase.printf("payload: %s\n", aResult.c_str());
    }
}
