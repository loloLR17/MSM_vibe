from pathlib import Path
import hashlib,json
p=Path('/home/lolo/dev/msm/reports/TR2-windows-trust-live-resume');w=Path('/mnt/c/Users/lolo/AppData/Local/MSM/TR2-LIVE-DEMO-20261019');r=Path('/home/lolo/dev/msm/projects/MSM_vibe-Windows-trust-live-resume/Modbus RTU/Supervision TR2/operator-demo');checks=[]
for f in sorted(r.glob('*.cmd')):
 a=(w/f.name).read_bytes();b=f.read_bytes();assert a.replace(b'\r\n',b'\n')==b
 checks.append(dict(name=f.name,installed_sha256=hashlib.sha256(a).hexdigest(),source_sha256=hashlib.sha256(b).hexdigest(),difference='CRLF on Windows vs LF in Git only',content_equal_after_CRLF_normalization=True))
f=w/'LIRE_AVANT_DEMO.txt';assert not f.exists()
f.write_text('''TR2 LIVE - qualification limitée c36c971 du 19/10/2026

Firmware LIVE conservé sur TR2-001, arrêté en fin d’essai.
Deux START/STOP réussis. Second run : au moins 11 min 50,7 s.
1. LANCER_SERVICE.cmd : garder la fenêtre ouverte.
2. OUVRIR_EDGE.cmd : page LIVE, http://127.0.0.1:5100
3. START.cmd : attendre terminal_and_effect_observed, code retour 0 (~40 s).
4. STOP.cmd : attendre terminal ET HELD (~40 s). La demande seule ne prouve pas l’arrêt.
5. ARRETER_SERVICE.cmd seulement après STOP confirmé.

Ne pas lancer deux commandes simultanées ou un autre maître COM4.
La pause de lecture crée une interruption visible, sans mesures inventées.
Limite connue : l’acquittement de pause peut osciller ; si timeout/refus COM,
ne rien forcer ni modifier les fichiers de contrôle, conserver les journaux.
Ne pas fermer le service seul pour arrêter une acquisition.
Ne pas supprimer/modifier data/transactions.json ou remplacer des DLL.
Les données/logs physiques sont dans data/. Le profil est diagnostique partiel.
La prochaine version compilée n’est pas automatiquement acceptée par Windows.
La signature durable est un chantier distinct ; aucune protection à désactiver.

Rapport et preuves : https://github.com/loloLR17/MSM_vibe/blob/main/Modbus%20RTU/00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md
''',encoding='utf-8')
(p/'installed-operator-guide.json').write_text(json.dumps(dict(path=str(f),sha256=hashlib.sha256(f.read_bytes()).hexdigest(),launchers=checks,application_files_unchanged=377,new_document_only=True,initial_byte_comparison_rejected_due_only_to_documented_Windows_CRLF=True),indent=2))
(p/'operator-guide.txt').write_bytes(f.read_bytes());print('Five launchers unchanged; CRLF-only vs Git. Added text guide outside app only')
