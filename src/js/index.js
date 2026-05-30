/*
 * PyramidWatch — PebbleKit JS
 * AppMessage keys:
 *   0 = BG_CHOICE   : 0=Cream  1=Black  2=White
 *   1 = STAT_STYLE  : 0=Short  1=Full
 *   2 = STAT_1      : 0=Steps 1=HR 2=Sleep 3=Calories 4=Distance
 *   3 = STAT_2
 *   4 = STAT_3
 *   5 = STAT_4
 */

function loadCfg() {
  return {
    bg:          +(localStorage.getItem('pw_bg')    || '0'),
    statStyle:   +(localStorage.getItem('pw_ss')    || '0'),
    dateStyle:   +(localStorage.getItem('pw_ds')    || '0'),
    layoutMode:  +(localStorage.getItem('pw_lm')    || '0'),
    pyrPos:      +(localStorage.getItem('pw_pp')    || '1'),
    stat1:       +(localStorage.getItem('pw_s1')    || '0'),
    stat2:       +(localStorage.getItem('pw_s2')    || '1'),
    stat3:       +(localStorage.getItem('pw_s3')    || '2'),
    stat4:       +(localStorage.getItem('pw_s4')    || '3')
  };
}

function saveCfg(c) {
  localStorage.setItem('pw_bg', c.bg);
  localStorage.setItem('pw_ss', c.statStyle);
  localStorage.setItem('pw_ds', c.dateStyle);
  localStorage.setItem('pw_lm', c.layoutMode);
  localStorage.setItem('pw_pp', c.pyrPos);
  localStorage.setItem('pw_s1', c.stat1);
  localStorage.setItem('pw_s2', c.stat2);
  localStorage.setItem('pw_s3', c.stat3);
  localStorage.setItem('pw_s4', c.stat4);
}

function sendMsg(c) {
  Pebble.sendAppMessage(
    { '0': c.bg, '1': c.statStyle, '2': c.stat1, '3': c.stat2,
      '4': c.stat3, '5': c.stat4, '6': c.dateStyle,
      '7': c.layoutMode, '8': c.pyrPos },
    function() { console.log('PyramidWatch: sent ok'); },
    function(e) { console.log('PyramidWatch: failed', JSON.stringify(e)); }
  );
}

var STAT_OPTS = ['Steps','Heart Rate','Sleep','Calories','Distance'];

function buildConfig(c) {
  function radio(name, opts, sel) {
    return opts.map(function(l,i){
      return '<label class="opt"><input type="radio" name="'+name+'" value="'+i+'"'+(i===sel?' checked':'')+'>'+
             '<span>'+l+'</span></label>';
    }).join('');
  }
  function select(name, opts, sel) {
    return '<select name="'+name+'" class="sel">'+
      opts.map(function(l,i){ return '<option value="'+i+'"'+(i===sel?' selected':'')+'>'+l+'</option>'; }).join('')+
      '</select>';
  }

  var h = '<!DOCTYPE html><html><head>'
    + '<meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">'
    + '<style>'
    + 'body{margin:0;font:15px/1.6 -apple-system,sans-serif;background:#f2f1ed;color:#1a1a1a;padding:20px 20px 40px}'
    + 'h3{font-size:10px;text-transform:uppercase;letter-spacing:.12em;color:#8a7060;margin:24px 0 8px}'
    + 'h3:first-child{margin-top:0}'
    + '.opt{display:flex;align-items:center;gap:12px;background:rgba(255,255,255,0.6);border-radius:8px;padding:13px;margin:5px 0;cursor:pointer;border:1.5px solid transparent}'
    + '.opt:has(input:checked){border-color:#321c14;background:rgba(255,255,255,0.9)}'
    + '.opt input{accent-color:#321c14;width:18px;height:18px;flex-shrink:0;margin:0}'
    + '.opt span{font-size:14px;color:#1a1a1a}'
    + '.swatch{width:22px;height:22px;border-radius:4px;flex-shrink:0;border:1px solid rgba(0,0,0,0.15)}'
    + '.cream{background:#f2f1ed}.black{background:#0a0a0a}.white{background:#fafafa}'
    + '.sel{width:100%;padding:12px;border-radius:8px;border:1.5px solid #d0c4b8;background:#fff;'
    +      'font-size:14px;color:#1a1a1a;margin:4px 0;-webkit-appearance:none;appearance:none}'
    + '.row{display:flex;align-items:center;gap:10px;margin:5px 0}'
    + '.row label{font-size:12px;color:#8a7060;min-width:48px}'
    + '#s{display:block;width:100%;padding:14px;background:#321c14;color:#f2f1ed;border:none;'
    +    'border-radius:8px;font-size:14px;letter-spacing:.06em;text-transform:uppercase;margin-top:28px;cursor:pointer;box-sizing:border-box}'
    + '</style></head><body>'

    + '<h3>Background</h3>'
    + '<label class="opt"><input type="radio" name="bg" value="0"'+(c.bg===0?' checked':'')+'><div class="swatch cream"></div><span>ePaper Cream</span></label>'
    + '<label class="opt"><input type="radio" name="bg" value="1"'+(c.bg===1?' checked':'')+'><div class="swatch black"></div><span>ePaper Black</span></label>'
    + '<label class="opt"><input type="radio" name="bg" value="2"'+(c.bg===2?' checked':'')+'><div class="swatch white"></div><span>ePaper White</span></label>'

    + '<h3>Layout mode</h3>'
    + radio('layoutMode', ['Standard — full-width pyramid', 'Wrap — smaller pyramid, text flows around'], c.layoutMode)

    + '<h3>Pyramid position <span style="font-size:10px;color:#8a7060">(wrap mode)</span></h3>'
    + radio('pyrPos', ['Left', 'Center', 'Right'], c.pyrPos)

    + '<h3>Date font style</h3>'
    + radio('dateStyle', ['Uniform — same size throughout', 'Scaled — larger toward bottom'], c.dateStyle)

    + '<h3>Stat label style</h3>'
    + radio('statStyle', ['Short — STP / HR / SL / CAL', 'Full — Steps / Heart Rate / Sleep / Calories'], c.statStyle)

    + '<h3>Health stats</h3>'
    + '<div class="row"><label>Stat 1</label>'+select('stat1', STAT_OPTS, c.stat1)+'</div>'
    + '<div class="row"><label>Stat 2</label>'+select('stat2', STAT_OPTS, c.stat2)+'</div>'
    + '<div class="row"><label>Stat 3</label>'+select('stat3', STAT_OPTS, c.stat3)+'</div>'
    + '<div class="row"><label>Stat 4</label>'+select('stat4', STAT_OPTS, c.stat4)+'</div>'

    + '<button id="s">Save</button>'
    + '<script>'
    + 'function g(n){var e=document.querySelector("input[name="+n+"]:checked");return e?+e.value:0;}'
    + 'function gs(n){var e=document.querySelector("select[name="+n+"]");return e?+e.value:0;}'
    + 'document.getElementById("s").onclick=function(){'
    +   'location.href="pebblejs://close#"+encodeURIComponent(JSON.stringify({'
    +   'bg:g("bg"),statStyle:g("statStyle"),dateStyle:g("dateStyle"),'
    +   'layoutMode:g("layoutMode"),pyrPos:g("pyrPos"),'
    +   'stat1:gs("stat1"),stat2:gs("stat2"),stat3:gs("stat3"),stat4:gs("stat4")}));'
    + '};<\/script></body></html>';

  return 'data:text/html,'+encodeURIComponent(h);
}

Pebble.addEventListener('ready', function() {
  console.log('PyramidWatch: ready');
  sendMsg(loadCfg());
});

Pebble.addEventListener('showConfiguration', function() {
  Pebble.openURL(buildConfig(loadCfg()));
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (!e||!e.response||e.response===''||e.response==='CANCELLED') return;
  var raw=e.response;
  if (raw.indexOf('#')!==-1) raw=raw.substring(raw.lastIndexOf('#')+1);
  var c; try{c=JSON.parse(decodeURIComponent(raw));}catch(err){return;}
  saveCfg(c);
  sendMsg(c);
});
