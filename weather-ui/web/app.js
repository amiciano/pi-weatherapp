// web/app.js

function f(n) {
  if (n === null || n === undefined || Number.isNaN(n)) return "—";
  return Math.round(Number(n));
}

function iconUrl(iconField) {
  // JSON gives: "//cdn.weatherapi.com/..."
  if (!iconField) return "";
  if (iconField.startsWith("//")) return "https:" + iconField;
  return iconField;
}

function niceTime(iso) {
  // "2026-01-16T13:45:50"
  if (!iso) return "—";
  const d = new Date(iso);
  if (Number.isNaN(d.getTime())) return iso;
  return d.toLocaleString(undefined, { weekday: "short", hour: "numeric", minute: "2-digit" });
}

async function load() {
  const res = await fetch("/data/weather.json", { cache: "no-store" });
  const j = await res.json();

  // Top
  const city = j.location?.name ?? j.location?.city ?? j.location?.region ?? "—";
  document.getElementById("loc").textContent = city;

  document.getElementById("temp").textContent = `${f(j.current?.temp)}°`;
  document.getElementById("cond").textContent = j.current?.condition?.text?.trim() || "—";

  // Cards
  document.getElementById("high").textContent = `${f(j.current?.high)}°`;
  document.getElementById("low").textContent  = `${f(j.current?.low)}°`;
  document.getElementById("feels").textContent = `${f(j.current?.feelslike)}°`;
  document.getElementById("rain").textContent = `${f(j.current?.rain)}%`;

  document.getElementById("wind").textContent = `${f(j.current?.wind)} mph`;
  document.getElementById("gusts").textContent = `${f(j.current?.gusts)} mph`;
  document.getElementById("hum").textContent = `${f(j.current?.humidity)}%`;
  document.getElementById("uv").textContent = `${f(j.current?.uv)}`;

  document.getElementById("updated").textContent = niceTime(j.last_updated);

  // Current icon
  const curIcon = iconUrl(j.current?.condition?.icon);
  const curIconEl = document.getElementById("curIcon");
  if (curIcon) {
    curIconEl.src = curIcon;
    curIconEl.style.display = "block";
  } else {
    curIconEl.style.display = "none";
  }

  // Forecast row
  const fc = document.getElementById("forecast");
  fc.innerHTML = "";

  const days = Array.isArray(j.forecast) ? j.forecast : [];
  for (const d of days.slice(0, 6)) {
    const el = document.createElement("div");
    el.className = "day";

    const icon = iconUrl(d.condition?.icon);
    el.innerHTML = `
      <div class="dow">${(d.day ?? "—").trim()}</div>
      <img class="wxicon" src="${icon}" alt="" onerror="this.style.display='none'"/>
      <div class="small">${(d.condition?.text ?? "—").trim()}</div>
      <div class="hi-lo">${f(d.high)}° / ${f(d.low)}°</div>
      <div class="tiny">Rain: ${f(d.rain)}%</div>
    `;
    fc.appendChild(el);
  }
}

load();
setInterval(load, 30_000); // refresh every 30 seconds