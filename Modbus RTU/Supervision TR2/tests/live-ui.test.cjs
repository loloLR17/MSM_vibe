const fs = require('node:fs');
const vm = require('node:vm');
const assert = require('node:assert/strict');
const source = fs.readFileSync(process.argv[2], 'utf8');
class Element {
  constructor() { this.children = []; this.textContent = ''; }
  append(child) { this.children.push(child); }
  replaceChildren() { this.children = []; this.textContent = ''; }
  allText() { return [this.textContent, ...this.children.map(c => c.allText())].join('\n'); }
}
async function check(label, views, expected, forbidden, ok = true) {
  const elements = {liveRows: new Element(), liveError: new Element()};
  const context = {document: {getElementById: id => elements[id], createElement: () => new Element()},
    fetch: async () => ({ok, status: ok ? 200 : 503, json: async () => views}), setTimeout: () => {}};
  vm.createContext(context); vm.runInContext(source, context); await new Promise(setImmediate);
  const text = elements.liveRows.allText() + elements.liveError.allText();
  assert(text.includes(expected), label + ': missing ' + expected);
  if (forbidden) assert(!text.includes(forbidden), label + ': misleading ' + forbidden);
  console.log('PASS', label);
}
const o = {deviceId: {value: 42}, identity: {profile: 'LiveDiagnostic'}, source: '<img src=x onerror=alert(1)>', receivedAt: '2026-10-17T12:00:00Z', continuity: 'Sequential', segmentId: 'PC segment only', snapshot: {held: false, acquisitionStopped: false, valuesValid: true, fresh: true, windowUtc: null, value: {calculationSequence: 1, valueAgeMilliseconds: 0, rmsGlobalMg: 958, rmsXMg: 955, rmsYMg: 75, rmsZMg: 13, peakGlobalMg: 998, peakXMg: 994, peakYMg: 110, peakZMg: 50}}};
const view = {observation: o, transportConnected: true, receptionStale: false, isCurrent: true, archiveState: 'Persisted'};
(async () => {
 await check('fresh with UTC unavailable', [view], 'Indisponible / non synchronisée', null);
 await check('held never current', [{...view, isCurrent: false, observation: {...o, snapshot: {...o.snapshot, held: true, fresh: false, acquisitionStopped: true}}}], 'HELD — dernière valeur retenue, non courante', 'Valeur valide et fraîche reçue');
 await check('PC stale overrides frame fresh', [{...view, receptionStale: true, isCurrent: false}], 'Réception périmée', 'Valeur valide et fraîche reçue');
 await check('rejected frame quarantined', [{...view, isCurrent: false, observation: {...o, continuity: 'Rejected', snapshot: null}}], 'valeurs en quarantaine', '958');
 await check('transport disconnected', [{...view, transportConnected: false, isCurrent: false}], 'Communication perdue', 'Valeur valide et fraîche reçue');
 await check('API failure clears old values', [], 'aucune valeur courante affichée', '958', false);
 await check('archive failure visible', [{...view, archiveState: 'Failed', archiveError: 'disk failed'}], 'Failed — disk failed', null);
 // All untrusted source/details are assigned with textContent, never HTML interpolation.
 assert(!source.includes('innerHTML'));
 console.log('PASS text-only rendering; 7 UI scenarios, simulated DOM (not a browser qualification)');
})().catch(e => { console.error(e); process.exitCode = 1; });
