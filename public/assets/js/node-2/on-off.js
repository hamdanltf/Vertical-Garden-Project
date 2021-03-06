        function funcoff() {
            firebase.database().ref("pupuk2").set({
                servo_2: "off"
            });
            Swal.fire(
                'Fertillizer OFF',
                ' ',
                'error'
            );
            getData();
        }

        function funcon() {
            firebase.database().ref("pupuk2").set({
                servo_2: "on"
            });
            Swal.fire(
                'Fertillizer ON',
                ' ',
                'success'
            );
            getData();
        }

        function funcoto() {
            firebase.database().ref("pupuk2").set({
                servo_2: "oto"
            });
            Swal.fire(
                'Auto Pilot Mode',
                'Auto Fertilizing every 3 months activated',
                'warning'
            );
            getData();
        }

        function getData() {
            firebase.database().ref('/').once('value', function (snapshot) {
                snapshot.forEach(function (childSnapshot) {
                    var childData = childSnapshot.val();
                    document.getElementById("data").innerHTML = "pupuk2: " + childData['servo_2'];
                })
            })
        }
        getData();