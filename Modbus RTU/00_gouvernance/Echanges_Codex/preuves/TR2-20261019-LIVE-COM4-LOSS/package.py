from pathlib import Path
import hashlib,json,zlib
p=Path('/home/lolo/dev/msm/reports/TR2-live-com4-loss-diag');r=Path('/home/lolo/dev/msm/projects/MSM_vibe-LIVE-COM4-loss-diag');o=r/'Modbus RTU/00_gouvernance/Echanges_Codex/preuves/TR2-20261019-LIVE-COM4-LOSS';o.mkdir(exist_ok=True);m=[]
for f in sorted(p.rglob('*')):
 if not f.is_file() or '__pycache__' in f.parts or f.name=='model-final' or f.name.startswith('remote-') and f.suffix=='.db':continue
 rel=f.relative_to(p).as_posix();b=f.read_bytes();compressed=len(b)>12000 or f.suffix in ('.db','.bin') or b'\r' in b
 stored=rel+'.zlib' if compressed else rel;data=zlib.compress(b,9) if compressed else b;dest=o/stored;dest.parent.mkdir(parents=True,exist_ok=True);dest.write_bytes(data);m.append(dict(source=rel,stored=stored,bytes=len(b),sha256=hashlib.sha256(b).hexdigest(),stored_sha256=hashlib.sha256(data).hexdigest(),compression='zlib' if compressed else None))
(o/'BUNDLE.json').write_text(json.dumps(m,indent=2)+'\n')
(o/'README.md').write_text('''# Preuves TR2-20261019-LIVE-COM4-LOSS-DIAG

Cause actuelle observée avant reset puis reproduite : GDB injectait un READ FRAM262144octets dans un callback chronométré ;846318µs original,846632µs reproduits, garde2500µs conservée. FIFO et transactions arrêtées, aucune écriture persistante pendant diagnostic causal. Deux resets nécessaires, zéroflash. NouveauSTART5/18/STOP5/19 et B0/B3réels après auditcorrigé sansreset ; serviceWindows laissé actif, cibleSTOP/HELD.

`BUNDLE.json` contient les empreintes brutes/stockées. `.zlib` = zlib.decompress ; aucun octet CRLF/binaire normalisé. `SHA256SUMS` couvre les fichiers versionnés. Logs/erreurs et état initial sont conservés, archives antérieures inchangées.

- `pc-initial/` + manifeste : septfichiers copiés avant reprise, ancienSQLite6336lignes fermé/intègre ;276CommunicationLost et260B0tx/zéroRX. Ledger17. Le fichier de l’opérateur14:19 n’a pas été retrouvé ; provenance utilisateur/contrat explicitée.
- `hardware/first-state-no-reset-no-call*` : première observation sansreset ni fonctioncible ; défaut6,service846318µs, IRQactives/RX présent. `fram-fresh-fault-preserved*` : FRAM entièree18e identique avant récupération.
- `hardware/reproduce-*`, `reproduced-*` : reproductionunique unsafeREAD dans u_service avecFIFOinactive ; mêmefaute,FRAMinchangée. Ce sont des recettes de diagnostic **à ne pas rejouer comme procédure opérateur**. Les deux resets de récupération sont consignés séparément.
- `safe-audit-before-commands/` et `safe-audit-final/` : lecture au prochainlive_runtime_step horsscope, gardesréelles/Flashc36exacte ; nouvelleprocédure finale avec refusprocessavantGDB. `refuse-active-service/` prouve le refus avant connexion et sans.gdb/.bin lorsque le service existe.
- `physical/restored-start*`/`restored-stop*` : seules nouvelles commandesB5 réelles,5/18et5/19 terminaux+effets ; deuxcapturesEdgeRUNNING80→146globaux,120→219XYZ, valeurs réelles. Pas de nouvelleendurance11min50s.
- `final-after-audit-B0-B3.json`, inventaireWindows : vraiesréponses aprèsauditfinal, sansreset, STOPHELD ;377fichiersappinchangés,SAC1/aucunblocagecorrélé/unservice/nodébugger. `pc-final-prefixes/` conserve des préfixes datés des logs actifs, pas une fin définitive.
- `live-final.db.zlib` : snapshotcohérent7713lignes via APIbackupSQLiteWindows, connexionreadonly ; anciens6336payloads exacts etjournauxutilisateur inchangés. Source de l’API en commentairedu script ; pas une copie bruteWAL active.
- `model-final*` : métadonnées simulées par cœurR2 réel, comparaisonFRAMcomplète physique exactecd1486…3406/journal102/root115/run14/config3/STOP5/19 ; préfixehistorique ettail exacts. Modèle≠effetphysique.
- `preservation.json`, étatGitinitial :8180fichiers/72autresworktrees/deuxstashes/mainhistorique82169af préservés. Firmware/PC inchangés, uniquementoutillage/docs corrigés.

Rapport courant pour résultats/limites/état laissé ; aucune qualification indépendanteChatGPT ou industrielle déduite de ces preuves. Lire les permissions de la mission active avant tout nouvel accès cible.
''')
(o/'SHA256SUMS').write_text(''.join(hashlib.sha256(f.read_bytes()).hexdigest()+'  '+f.relative_to(o).as_posix()+'\n' for f in sorted(o.rglob('*')) if f.is_file() and f.name!='SHA256SUMS'))
for x in m:
 b=(o/x['stored']).read_bytes();raw=zlib.decompress(b) if x['compression'] else b;assert hashlib.sha256(b).hexdigest()==x['stored_sha256'] and hashlib.sha256(raw).hexdigest()==x['sha256'] and len(raw)==x['bytes']
print('Packaged and verified',len(m),'files',sum(f.stat().st_size for f in o.rglob('*') if f.is_file()),'bytes')
