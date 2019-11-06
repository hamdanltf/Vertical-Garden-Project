        function funcoff() {
            firebase.database().ref("pupuk").set({
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
            firebase.database().ref("pupuk").set({
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
            firebase.database().ref("pupuk").set({
                servo_2: "on"
            });
            Swal.fire(
                'Fertillizer ON',
                'Otomatically off in 10 seconds',
                'warning'
            );
            getData();
        }

        function getData() {
            firebase.database().ref('/').once('value', function (snapshot) {
                snapshot.forEach(function (childSnapshot) {
                    var childData = childSnapshot.val();
                    document.getElementById("data").innerHTML = "Pupuk: " + childData['servo_2'];
                })
            })
        }
        getData();