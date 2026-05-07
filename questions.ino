/*
 * Question Deck - Bar/Cafe Edition
 * ESP8266 Captive Portal
 * 
 * Creates Wi-Fi AP "QuestionDeck" (open). Any connected device
 * is redirected to a web page that draws random questions.
 * 
 * 50 light icebreakers + 50 medium-deep questions (pub-friendly)
 */

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

// AP credentials
const char* ssid = "icebreaker";
const char* password = NULL;   // open network

const byte DNS_PORT = 53;
DNSServer dnsServer;
ESP8266WebServer server(80);

// Synchronized question state
unsigned long lastQuestionTime = 0;
const unsigned long QUESTION_DURATION = 60000; // 60 seconds
const unsigned int TOTAL_QUESTIONS = 60;
String currentQuestion = "";
String currentReader = "";
unsigned int questionsPresented = 0;
bool currentIsDeep = false;

// Session tracking for multi-device mode
#define MAX_CLIENTS 20
struct ClientSession {
  String id;
  unsigned long lastSeen;
};
ClientSession clients[MAX_CLIENTS];
int clientCount = 0;

// ---------- PROGMEM question arrays ----------
// Light questions (0-199)
const char* const lightQuestions[] PROGMEM = {
  "Best thing you've eaten in the past week?",
  "Fight one horse-sized duck or 100 duck-sized horses?",
  "Movie you can quote from memory?",
  "New skill you'd wake up with tomorrow?",
  "Worst haircut you've ever had?",
  "Smell that instantly makes you happy?",
  "Most useless talent you possess?",
  "Reality TV title for your life?",
  "Food you hated as a kid but love now?",
  "Worst gift you've given or received?",
  "Dinner with any fictional character?",
  "Purchase under $20 that improved your life?",
  "Best nap you've ever taken?",
  "If animals could talk, which would be most annoying?",
  "Word you always mispronounce or misspell?",
  "Trend you're embarrassed to have followed?",
  "Background character in which TV show?",
  "Something you believed as a child that seems silly now?",
  "Most unusual thing in your fridge?",
  "Instantly fluent in which language?",
  "Song that guarantees you'll dance/sing?",
  "Silliest thing you've gotten emotional about?",
  "Your warning label?",
  "Best free advice you've received?",
  "Hobby you tried and immediately quit?",
  "Rename the color blue?",
  "Something old-fashioned you do?",
  "Super specific thing that brings you joy?",
  "Rewind or pause button for your life?",
  "Best compliment you've received?",
  "Something your body does that's weird/funny?",
  "Live in any decade for one week?",
  "Subject you could talk about for an hour?",
  "Last thing you searched on your phone?",
  "If you opened a small business, what would it be?",
  "Most beautiful place you've been?",
  "Useless fact you love telling people?",
  "Surprisingly good at for no reason?",
  "Best \"that didn't go as planned\" story?",
  "Swap lives with a pet – what pet?",
  "Something you're currently procrastinating on?",
  "Weirdest dream you remember?",
  "Sound that makes you cringe?",
  "Expert in one useless Olympic sport?",
  "Something you collect without meaning to?",
  "Worst excuse for being late?",
  "Kitchen gadget you swear by?",
  "Describe yourself in three emojis.",
  "Something everyone should experience at least once?",
  "Question you wish more people would ask you (light version)?",
  "Conspiracy theory you secretly believe?",
  "Celebrity you'd want to be friends with?",
  "Weird food combination you actually like?",
  "If you were a vegetable, which would you be?",
  "Most random skill you've picked up?",
  "App you can't live without?",
  "Fictional world you'd want to live in?",
  "Best purchase you've made this year?",
  "Worst fashion choice you've made?",
  "If you had a warning label, what would it say?",
  "Superhero power you'd actually want?",
  "Most embarrassing moment you can laugh about?",
  "Pet peeve that's oddly specific?",
  "Talent you wish you had?",
  "Best concert or live show you've been to?",
  "Thing you're inexplicably good at?",
  "Weirdest compliment you've received?",
  "If you could be any animal for a day?",
  "Favorite type of cuisine?",
  "Most annoying notification sound?",
  "Thing you learned recently that blew your mind?",
  "Your go-to comfort food?",
  "Worst password you've ever used?",
  "If you were a wrestler, what would your name be?",
  "Most cringey song you love?",
  "Place you'd go if you could teleport?",
  "If your life was a movie genre?",
  "Weirdest place you've fallen asleep?",
  "App you wish existed?",
  "Most useless superpower?",
  "Ridiculous fear you actually have?",
  "If you could rename one thing, what would it be?",
  "Worst WiFi name you've seen?",
  "Most messed up autocorrect?",
  "If breakfast was your last meal?",
  "Most expensive thing you've broken?",
  "Celebrity crush you're not embarrassed about?",
  "Skill you're weirdly proud of?",
  "If your pet could talk?",
  "Most useless degree you'd get?",
  "Best smell in the world?",
  "Thing you do that would surprise people?",
  "If you could only eat at one restaurant?",
  "Worst roommate situation?",
  "Most random talent at a party?",
  "If you were a kitchen appliance?",
  "Best inside joke you have?",
  "Weirdest text you've sent?",
  "If you could be famous for anything?",
  "Most random fact you know?",
  "Thing you've Googled that you're not proud of?",
  "Best excuse you've ever made?",
  "If you could have any animal as a pet?",
  "Most embarrassing thing at work?",
  "Random thing that makes you happy?",
  "Worst lie you've told?",
  "If you were a coffee order?",
  "Most absurd talent show act?",
  "Thing you're irrationally angry about?",
  "Best prank you've pulled?",
  "If you were a dessert?",
  "Most embarrassing song on your playlist?",
  "Best autocorrect fail?",
  "If you were a time period?",
  "Weirdest thing you've searched?",
  "Best lazy day activity?",
  "Most random talent?",
  "If you were a weather pattern?",
  "Thing you've done that nobody knows?",
  "Best game show you'd win?",
  "Most useless skill?",
  "If you were a holiday?",
  "Worst job interview?",
  "Thing you're secretly good at?",
  "Most random collection?",
  "If you were a social media platform?",
  "Best moment of your week?",
  "Most annoying habit?",
  "If you were a sandwich?",
  "Thing you judge people for?",
  "Best excuse for being late?",
  "Most random fear?",
  "If you were a board game?",
  "Thing you've bought you never use?",
  "Best weird combo you've tried?",
  "Most random friendship?",
  "If you were a Netflix show?",
  "Thing you're addicted to?",
  "Best impulse purchase?",
  "Most random talent you have?",
  "If you were a cocktail?",
  "Thing you can't admit in public?",
  "Best moment of your life?",
  "Most random conversation?"
};

// Deep questions (0-199) – bar friendly
const char* const deepQuestions[] PROGMEM = {
  "Best advice you've ever gotten from a stranger?",
  "If your life had background music right now, what song?",
  "Something you've done in the past month that made you feel proud?",
  "Who in this group would you trust to order food for you?",
  "A small everyday mystery you've never solved?",
  "A luxury that's actually worth the money to you?",
  "If you could know the truth behind one local rumor, what?",
  "A subject you could argue about for fun, not anger?",
  "Something you changed your mind about recently because of someone?",
  "A simple pleasure you'd miss if it disappeared tomorrow?",
  "Small decision that changed your life in a big way?",
  "Skill you're trying to learn, and why?",
  "What does a perfect day look like for you?",
  "Someone you've never met but feel grateful to?",
  "Book/movie/song that changed your worldview?",
  "Compliment that stuck with you for years?",
  "Teach a one-hour class on anything – what?",
  "Accomplishment that surprised even you?",
  "Tradition from your culture/family that you love?",
  "Something you believed about adulthood that was false?",
  "Place you've never been that feels like home?",
  "Best life advice you've ignored?",
  "Kindest thing a stranger has done for you?",
  "One daily annoyance you'd instantly remove?",
  "Something you're curious about but know little about?",
  "A moment that felt like \"the calm before the storm\"?",
  "Who influenced your sense of humor the most?",
  "Something you used to be insecure about but now accept/love?",
  "Most meaningful compliment you've given someone?",
  "Something you've done that made you feel truly alive?",
  "Describe your personality in three nouns (not adjectives).",
  "What would you like to be known for someday?",
  "A risk that didn't work out but you're glad you took?",
  "What does \"success\" look like to you right now?",
  "A piece of art that moved you deeply?",
  "Topic of a 5-minute speech to 1,000 people?",
  "Something you've changed about your routine recently?",
  "A fear you've overcome?",
  "A small everyday thing that makes you feel like yourself?",
  "What would you want future generations to know about this time?",
  "Add a new room to your home – what for?",
  "A phrase or saying you live by?",
  "A hobby you picked up as an adult that surprised you?",
  "A moment when you felt like you belonged somewhere?",
  "Something you're looking forward to in the next six months?",
  "Thank a teacher/mentor – what for?",
  "A memory that always makes you smile?",
  "What's a question you wish someone in this group would ask you?",
  "What's something you want to be better at in five years?",
  "If you could have a superpower only for bar arguments, what would it be?",
  "When was the last time you felt really understood?",
  "What's a value that's become more important to you over time?",
  "Describe a moment you felt truly proud of yourself?",
  "What would your younger self think of you now?",
  "Is there something you've forgiven yourself for?",
  "What brings you a sense of calm?",
  "Tell about someone who saw potential in you?",
  "What's a goal that scared you but you pursued anyway?",
  "How have your priorities changed in the last few years?",
  "What's a lesson life has taught you?",
  "When did you realize something important about yourself?",
  "What's something you're passionate about but don't talk about?",
  "Describe your ideal life in one sentence?",
  "What makes you feel most like yourself?",
  "Is there something you wish you'd done differently?",
  "What's your definition of a good friend?",
  "When did you last feel inspired?",
  "What's a skill you're proud of?",
  "How do you want to be remembered?",
  "What's something you believed that wasn't true?",
  "Tell about a moment that changed your perspective?",
  "What's something you're still learning?",
  "Describe a person who has impacted your life?",
  "What would you do if you weren't afraid?",
  "What's a quality you admire in others?",
  "Tell about a time you stood up for something?",
  "What does loyalty mean to you?",
  "When have you felt most vulnerable?",
  "What's something you wish you'd told someone?",
  "Describe a place that feels like home?",
  "What's a mistake that taught you something valuable?",
  "Tell about a person who inspired you?",
  "What's something that makes you feel alive?",
  "How do you define happiness?",
  "What's a dream you haven't shared with many people?",
  "Describe a moment of pure joy?",
  "What's something you've had to accept about yourself?",
  "Tell about a time you felt truly grateful?",
  "What does home mean to you?",
  "What's something you're working on improving?",
  "Describe a relationship that shaped you?",
  "What's a belief you've strongly held?",
  "Tell about a time you helped someone?",
  "What makes you feel connected to others?",
  "What's something you want to be remembered for?",
  "Describe a moment of personal growth?",
  "What's a truth you've learned?",
  "Tell about someone you admire?",
  "What does integrity mean to you?",
  "What's something that terrifies you?",
  "Describe a moment you felt truly seen?",
  "What's a goal you're proud of pursuing?",
  "Tell about a time you overcame something?",
  "What makes you feel most like yourself?",
  "What's something you wish more people understood?",
  "Describe your biggest strength?",
  "What's a challenge you're currently facing?",
  "Tell about a time you felt truly supported?",
  "What does family mean to you?",
  "What's something you're grateful for today?",
  "Describe a moment you felt pride?",
  "What's a value you live by?",
  "Tell about someone who believed in you?",
  "What brings you peace?",
  "What's something you want to change about the world?",
  "Describe your biggest fear?",
  "What's a strength you've developed?",
  "Tell about a moment of clarity?",
  "What does love mean to you?",
  "What's something you're proud of?",
  "Describe a moment that defined you?",
  "What's a question you've always wanted answered?",
  "Tell about someone you're grateful for?",
  "What makes life meaningful to you?",
  "What's something you wish you were better at?"
};

String registerClient(String clientId) {
  unsigned long now = millis();

  for (int i = 0; i < clientCount; i++) {
    if (clients[i].id == clientId) {
      clients[i].lastSeen = now;
      return clientId;
    }
  }

  if (clientCount < MAX_CLIENTS) {
    clients[clientCount].id = clientId;
    clients[clientCount].lastSeen = now;
    clientCount++;
    return clientId;
  }

  int oldest = 0;
  for (int i = 1; i < MAX_CLIENTS; i++) {
    if (clients[i].lastSeen < clients[oldest].lastSeen) {
      oldest = i;
    }
  }
  clients[oldest].id = clientId;
  clients[oldest].lastSeen = now;
  return clientId;
}

void rotateQuestion() {
  if (clientCount == 0) {
    currentQuestion = "";
    currentReader = "";
    return;
  }
  currentReader = clients[random(clientCount)].id;
  float deepProbability = min(questionsPresented / (TOTAL_QUESTIONS * 1.0f), 0.75f);
  currentIsDeep = (random(100) / 100.0f) < deepProbability;
  if (currentIsDeep) {
    currentQuestion = getQuestion(deepQuestions, random(50));
  } else {
    currentQuestion = getQuestion(lightQuestions, random(50));
  }
  questionsPresented++;
}

unsigned long getTimeRemaining() {
  unsigned long elapsed = millis() - lastQuestionTime;
  if (elapsed >= QUESTION_DURATION) return 0;
  return QUESTION_DURATION - elapsed;
}

// Helper to copy PROGMEM string to String
String getQuestion(const char* const* array, int index) {
  char buffer[256];
  strcpy_P(buffer, (char*)pgm_read_ptr(&array[index]));
  return String(buffer);
}


// Send JSON array from PROGMEM
void sendQuestionArray(const char* const* arr, int size) {
  String json = "[";
  for (int i = 0; i < size; i++) {
    if (i > 0) json += ",";
    String q = getQuestion(arr, i);
    q.replace("\\", "\\\\");
    q.replace("\"", "\\\"");
    json += "\"" + q + "\"";
  }
  json += "]";
  server.send(200, "application/json", json);
}

// Simplified HTML - questions only
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>Question Deck</title><style>*{margin:0;padding:0;box-sizing:border-box}body{background:#000;color:#fff;font-family:Arial,sans-serif;display:flex;justify-content:center;align-items:center;min-height:100vh;padding:20px}#counter{position:fixed;top:20px;right:20px;font-size:1.2rem;color:#a8dadc;z-index:100}#main{width:100%;max-width:600px;text-align:center}#question{font-size:3rem;font-weight:bold;min-height:200px;display:flex;align-items:center;justify-content:center;margin-bottom:40px;line-height:1.2;word-wrap:break-word}#question.listen{color:#87ceeb;font-size:2rem;font-weight:normal;font-style:italic}#timer{font-size:1.8rem;font-weight:bold;margin-top:30px;color:#ffd700}@media (max-width:600px){#question{font-size:2rem;min-height:150px}#question.listen{font-size:1.5rem}#timer{font-size:1.5rem}}</style></head><body><div id="counter">60/0</div><div id="main"><div id="question">Waiting...</div><div id="timer"></div></div><script>let sessionId=localStorage.getItem('sessionId')||'';const lightListenMsgs=['Pay attention','Lean in close','Focus up','Listen closely','Get ready','Stay tuned','This is good','Pay it forward'];const deepListenMsgs=['This is profound','Listen closely','Pay attention','Lean in','What wisdom','This matters','Remember this','Let it sink in'];function pickListenMsg(isDeep){const msgs=isDeep?deepListenMsgs:lightListenMsgs;return msgs[Math.floor(Math.random()*msgs.length)]}let currentListenMsg='';let currentQuestion='';let prevRemaining=15000;let wakeLock=null;const listenMessages=['Pay attention','Your moment is coming','Lean in close','Don\'t miss this','Focus up','What will they say?','Listen closely','This is good','Your turn soon','Get ready','Watch and learn','Here we go','Pay it forward','Your answer matters','Save your thoughts','Remember this','You\'re next','Stay tuned','This counts','Golden words'];async function requestWakeLock(){try{if('wakeLock' in navigator){wakeLock=await navigator.wakeLock.request('screen');document.addEventListener('visibilitychange',()=>{if(document.hidden&&wakeLock){wakeLock.release();wakeLock=null}else if(!document.hidden&&!wakeLock){requestWakeLock()}})}}catch(err){console.log('Wake lock failed',err)}}function registerSession(){fetch('/api/register?id='+sessionId).then(r=>r.json()).then(d=>{sessionId=d.id;localStorage.setItem('sessionId',sessionId);updateState()}).catch(e=>console.error(e))}function updateState(){fetch('/api/questions?session='+sessionId).then(r=>r.json()).then(d=>{const qEl=document.getElementById('question');const remaining=Math.ceil(d.remaining/1000);document.getElementById('timer').innerText=remaining+'s';document.getElementById('counter').innerText='60/'+d.count;if(d.active){qEl.innerText=d.question;qEl.classList.remove('listen');currentQuestion=d.question}else{if(d.question!==currentQuestion){currentListenMsg=pickListenMsg(d.isDeep);currentQuestion=d.question}qEl.innerText=currentListenMsg;qEl.classList.add('listen')}prevRemaining=remaining}).catch(e=>console.error(e))}registerSession();requestWakeLock();setInterval(updateState,500);</script></body></html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  // Start Wi-Fi in AP mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  Serial.print("AP IP: ");
  Serial.println(WiFi.softAPIP());
  
  // DNS captive portal
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  
  // Web routes
  server.on("/", []() {
    server.send_P(200, "text/html", index_html);
  });
  
  // Register or update client
  server.on("/api/register", []() {
    String clientId = server.arg("id");
    if (clientId.length() == 0) {
      clientId = "c" + String(random(1000000));
    }
    registerClient(clientId);
    server.send(200, "application/json", "{\"id\":\"" + clientId + "\"}");
  });

  server.on("/api/questions", []() {
    String clientId = server.arg("session");

    if (clientId.length() > 0) {
      registerClient(clientId);
    }

    if (currentQuestion.length() == 0 && clientCount > 0) {
      rotateQuestion();
    }

    bool isActive = (clientId == currentReader);
    String q = currentQuestion;
    q.replace("\\", "\\\\");
    q.replace("\"", "\\\"");

    unsigned long remaining = getTimeRemaining();

    String response = "{\"active\":" + String(isActive) + ",\"question\":\"" + q + "\",\"remaining\":" + String(remaining) + ",\"count\":" + String(questionsPresented) + ",\"isDeep\":" + String(currentIsDeep) + "}";
    server.send(200, "application/json", response);
  });
  
  server.onNotFound([]() {
    server.sendHeader("Location", "/", true);
    server.send(302, "text/plain", "");
  });
  
  server.begin();
  Serial.println("Server started");

  // Initialize first question
  lastQuestionTime = millis();
  rotateQuestion();
}

void loop() {
  unsigned long now = millis();

  // Rotate question every 15 seconds
  if (now - lastQuestionTime >= QUESTION_DURATION) {
    lastQuestionTime = now;
    rotateQuestion();
  }

  dnsServer.processNextRequest();
  server.handleClient();
}