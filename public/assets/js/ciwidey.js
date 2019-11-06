src = "https://www.gstatic.com/firebasejs/7.2.3/firebase-app.js"

const TelegramBot = require('node-telegram-bot-api');
const token = '763668315:AAH0raUR7Yt9BMd7usVlGVsY4jwVuPuK8ns';
const bot = new TelegramBot(token, {
    polling: true
});
const firebase = require('firebase');

var firebaseConfig = {
    apiKey: "AIzaSyA6pqDblEmNwYkothwwubDVd4kOgBrxlfU",
    authDomain: "verticalgarden-project.firebaseapp.com",
    databaseURL: "https://verticalgarden-project.firebaseio.com",
    projectId: "verticalgarden-project",
    storageBucket: "verticalgarden-project.appspot.com",
    messagingSenderId: "958789288546",
    appId: "1:958789288546:web:0de0a8fe1a230126e0073b",
    measurementId: "G-GZBVEKTCMY"
};
// Initialize Firebase
firebase.initializeApp(firebaseConfig);

bot.on('message', (msg) => {

    var Hi = "hi";
    if (msg.text.toString().toLowerCase().indexOf(Hi) === 0) {
        bot.sendMessage(msg.chat.id, "Hello dear user");
    }

});

var db = firebase.database()

async function main() {
    let FETCH_STATUS = false

    db.ref('/vertical_garden').endAt().limitToLast(1).on('child_added', function (snapshot) {
        if (FETCH_STATUS) {
            const {
                parameter
            } = snapshot.val()

            if (parameter.node == 'node-01') {
                if (parameter.suhu >= 30) {
                    bot.sendMessage(562592345, "Suhu node-1 sudah melebihi 30 derajat, Penyiraman aktif")
                }
            }

            if (parameter.node == 'node-02') {
                if (parameter.suhu >= 30) {
                    bot.sendMessage(562592345, "Suhu node-2 sudah melebihi 30 derajat, Penyiraman aktif")
                }
            }

        }
        FETCH_STATUS = true
    })
}

main()