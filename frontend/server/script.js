var za = document.getElementsByClassName("predzv_checkbox")[0]
var za_input = document.getElementsByClassName("za_input")[0]
var add = document.getElementsByClassName("day__buttons_button")[0]
var remove = document.getElementsByClassName("day__buttons_button")[1]
var save = document.getElementsByClassName("day__buttons_button")[2]
var day = document.getElementsByClassName("day__date_select")[0]
var done = document.getElementsByClassName("footer_button")[0]

var day_cnt = {"Monday":0, "Tuesday":0, "Wednesday":0, "Thursday":0, "Friday":0, "Saturday":0, "Sunday":0};
var day_zv = {"Monday":[], "Tuesday":[], "Wednesday":[], "Thursday":[], "Friday":[], "Saturday":[], "Sunday":[]};
var day_za = {"Monday":[false, 1], "Tuesday":[false, 1], "Wednesday":[false, 1], "Thursday":[false, 1], "Friday":[false, 1], "Saturday":[false, 1], "Sunday":[false, 1]}

za_input.addEventListener("change", change_za_input);
function change_za_input() {
	console.info();
	if(za_input.value=="" || isNaN(Number(za_input.value)) || Number(za_input.value) > 10 || za_input.value==0) {
		za_input.value=1;
	}
}

za_input.addEventListener("focus", focus_za_input);
function focus_za_input() {
	if(za_input.value=="" || za_input.value=="1") {
		za_input.value="";
	}
}

za_input.addEventListener("focusout", unfocus_za_input);
function unfocus_za_input() {
	if(za_input.value=="") {
		za_input.value=1;
	}
}


add.addEventListener("click", add_lesson);
function add_lesson() {
	day_cnt[day.value]++;

	var pr = document.createElement("p");
	pr.classList.add("day__schedule_p");
	var lesson_num = document.createTextNode(String(day_cnt[day.value]) + " урок: ");
	pr.appendChild(lesson_num);

	var input = document.createElement("input")
	input.classList.add("day__schedule_input");
	input.required = true;
	input.type = "time";
	pr.appendChild(input);
	
	var dash = document.createTextNode(" - ");
	pr.appendChild(dash);

	var input2 = document.createElement("input")
	input2.classList.add("day__schedule_input");
	input2.required = true;
	input2.type = "time";
	pr.appendChild(input2);

	var element = document.getElementsByClassName("day__schedule")[0];
	var child = document.getElementsByClassName("day__schedule_p")[day.value];
	element.insertBefore(pr, child);

	console.info(day_cnt);
}

remove.addEventListener("click", remove_lesson);
function remove_lesson() {
	if(day_cnt[day.value]>0) {
		document.getElementsByClassName("day__schedule_p")[day_cnt[day.value]-1].remove();
		day_cnt[day.value]--;
		console.info(day_cnt);
	}
}

save.addEventListener("click", save_lessons);
function save_lessons() {
	day_zv[day.value] = [];
	for(var i=1;i<=(day_cnt[day.value]*2);i++) {
		if(String(document.getElementsByClassName("day__schedule_input")[i-1].value) != "") {
			if(i%2==0) {
				if(String(document.getElementsByClassName("day__schedule_input")[i-1].value)[0]=="0") {
					day_zv[day.value].push(String(document.getElementsByClassName("day__schedule_input")[i-1].value).slice(1));
				}
				else {
					day_zv[day.value].push(String(document.getElementsByClassName("day__schedule_input")[i-1].value));
				}
			}
			else {
				if(za.checked == true) {
					day_zv[day.value].push(time_minus(String(document.getElementsByClassName("day__schedule_input")[i-1].value), ("0:0"+za_input.value)));
				}
				if(String(document.getElementsByClassName("day__schedule_input")[i-1].value)[0]=="0") {
					day_zv[day.value].push(String(document.getElementsByClassName("day__schedule_input")[i-1].value).slice(1));
				}
				else {
					day_zv[day.value].push(String(document.getElementsByClassName("day__schedule_input")[i-1].value));
				}
			}
		}
	}
	day_za[day.value] = [za.checked, Number(za_input.value)];

	create_alert("Сохранено!", "Расписание на выбранный день сохранено.")

	console.info(day_zv);
	console.info(day_za);
}

function sleep(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

async function create_alert(title_text, p) {
	while(document.getElementsByClassName("alert").length != 0) {
		document.getElementsByClassName("alert")[0].remove();
	}

	var div = document.createElement("div");
	div.classList.add("alert");

	var title = document.createElement("h3");
	title.innerHTML = title_text;
	title.classList.add("alert__header");
	div.append(title);

	var pr = document.createElement("p");
	pr.innerHTML = p;
	pr.classList.add("alert_p");
	div.append(pr);

	document.getElementsByClassName("under-footer")[0].appendChild(div);
	
	
	var alert = document.querySelector(".alert");
	alert.animate([{transform: "translate(40vh)" },{transform: "translate(-5vh)" },], {duration: 200,iterations: 1,});
	alert.style.transform = "translate(-5vh, 0)";
	
	await sleep(2500);

	alert.animate([{transform: "translate(-5vh)" },{transform: "translate(40vh)" }], {duration: 200,iterations: 1,});
	alert.style.transform = "translate(40vh, 0)";

	await sleep(200);
	alert.remove();
}

function time_minus(a, b) {
	if(a.length == 4) {
		var a_mins = Number(a.slice(2));
		var a_hrs = Number(a.slice(0, 1));
	}
	else {
		var a_mins = Number(a.slice(3));
		var a_hrs = Number(a.slice(0, 2));
	}

	if(b.length == 4) {
		var b_mins = Number(b.slice(2));
		var b_hrs = Number(b.slice(0, 1));
	}
	else {
		var b_mins = Number(b.slice(3));
		var b_hrs = Number(b.slice(0, 2));
	}

	ans_mins = String(((a_hrs*60+a_mins)-(b_hrs*60+b_mins))%60);
	ans_hrs = String(Math.floor(((a_hrs*60+a_mins)-(b_hrs*60+b_mins))/60));

	if(ans_mins.length==1) {
		var ans = ans_hrs + ":0" + ans_mins;
	}
	else {
		var ans = ans_hrs + ":" + ans_mins;
	}
	
	return ans;
}

day.addEventListener('change', change_day);
function change_day() {
	while(document.getElementsByClassName("day__schedule_p").length != 0) {
		document.getElementsByClassName("day__schedule_p")[0].remove();
	}
	za.checked = day_za[day.value][0];
	za_input.value = day_za[day.value][1];

	for(var i=0;i<day_cnt[day.value];i++) {
		var pr = document.createElement("p");
		pr.classList.add("day__schedule_p");
		var lesson_num = document.createTextNode(String(i+1) + " урок: ");
		pr.appendChild(lesson_num);
	
		var input = document.createElement("input")
		input.classList.add("day__schedule_input");
		input.required = true;
		input.type = "time";
		pr.appendChild(input);
		
		var dash = document.createTextNode(" - ");
		pr.appendChild(dash);
	
		var input2 = document.createElement("input")
		input2.classList.add("day__schedule_input");
		input2.required = true;
		input2.type = "time";
		pr.appendChild(input2);
	
		var element = document.getElementsByClassName("day__schedule")[0];
		var child = document.getElementsByClassName("day__schedule_p")[day.value];
		element.insertBefore(pr, child);
	}

	if(day_za[day.value][0]==false) {
		for(var i=0;i<day_cnt[day.value]*2;i++) {
			if(day_zv[day.value][i].length==5) {
				document.getElementsByClassName("day__schedule_input")[i].value = day_zv[day.value][i];
			}
			else if(day_zv[day.value][i].length==4) {
				document.getElementsByClassName("day__schedule_input")[i].value = "0" + day_zv[day.value][i];
			}
		}
	}
	else {
		var j = 0;
		for(var i=0;i<day_cnt[day.value]*3;i++) {
            if(i%3!=0) {
				if(day_zv[day.value][i].length==5) {
					document.getElementsByClassName("day__schedule_input")[j].value = day_zv[day.value][i];
				}
				else if(day_zv[day.value][i].length==4) {
					document.getElementsByClassName("day__schedule_input")[j].value = "0" + day_zv[day.value][i];
				}
				j++;
			}
        }
	}
}


done.addEventListener("click", done_lessons);
function done_lessons() {
	console.info(JSON.stringify(day_zv));
	navigator.clipboard.writeText(JSON.stringify(day_zv));

	create_alert("Успешно!", "Рассписание скопированно в буфер обмена.");
}