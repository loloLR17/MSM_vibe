# SSH HP temporaire — SSH-HP-001

Outil T1000 WSL Debian → client OpenSSH **Windows** → HP Windows PowerShell 5.1.
Il utilise l'alias Windows existant `tr2-hp`, une clé dédiée et la clé hôte déjà
connue. Aucun secret/configuration SSH n'est copié ou ajouté au dépôt. Le client
Linux n'a pas cet alias. Identité distante exigée : `LAPTOP-3F6J7STJ`.

Depuis la racine du dépôt, Python3 (bibliothèque standard seulement) :

```bash
python3 00_Environnement_developpement/T1000/ssh-hp/run_hp.py Probe \
  --evidence /home/lolo/dev/msm/reports/SSH-HP-probe-NOUVEAU
python3 00_Environnement_developpement/T1000/ssh-hp/run_hp.py Inventory \
  --evidence /home/lolo/dev/msm/reports/SSH-HP-inventory-NOUVEAU
python3 00_Environnement_developpement/T1000/ssh-hp/run_hp.py Offline \
  --evidence /home/lolo/dev/msm/reports/SSH-HP-offline-NOUVEAU \
  --exe 'C:\Users\Lolo\AppData\Local\TR2\M014-B-20261011-195242\TR2.Supervision.Service.exe' \
  --sha256 13228dfe3d30cf01829c16be1be22f30b061cc0cb0916b2e6a499b8fd1f96872
python3 -m unittest discover -s 00_Environnement_developpement/T1000/ssh-hp -v
```

Choisir un dossier de preuves neuf à chaque commande : un dossier existant est
refusé, même vide. L'exemple du chemin HP est **observé le11/10/2026**, le revérifier
avec Inventory s'il change. Inventory cherche à profondeur4 dans Desktop,
Documents, Downloads et LocalAppData/TR2 ; ce n'est pas un inventaire exhaustif
ni une preuve d'absence d'un projet ailleurs. .NET introuvable dans PATH ne prouve
pas son absence partout ; le paquet autonome s'exécute sans runtime installé.

`Probe` restitue identité, cwd, PowerShell et privilèges. `FailureProbe` retourne17
volontairement pour tester la propagation ; ce n'est pas un test Modbus.
`Inventory` lit PnP COM7, processus candidats, artefacts, service/config SSH et
`sshd -T` (politique globale, pas résolution exhaustive des clauses Match).
Aucune ouverture série. Aucun parcours de clés privées ni lecture de leur contenu.

`Offline` exige une empreinte EXE approuvée M014-A ou M014-D, vérifie celle du
manifeste correspondant puis les370 tailles/empreintes et les chemins contenus
dans le dossier. L'EXE est lancé avec une liste fixe `--diagnostic ...`, **sans
activation série**. Aucun argument arbitraire, polling, B5, écriture, retry ou
mode physique n'est exposé. Le journal produit est l'enveloppe SSH locale et les
sorties du programme ; le diagnostic OFFLINE ne crée pas de journal de lecture.
Le contrôle ne prétend pas éliminer une modification malveillante concurrente
entre vérification et lancement ; le compte et ses fichiers doivent être fiables.

Mutex Windows `Global\TR2-COM7-Orchestration` non bloquant + refus si un processus
TR2/dotnet/python/putty/terminal/modbus est détecté. Le mutex est libéré à la fin,
même sur erreur. Il protège les orchestrations coopérantes uniquement. La liste
ne recense pas tous les handles Windows, et une course reste possible avec un
programme externe : **COM7 disponible/exclusif n'est pas établi**. Aucun port
n'est ouvert pour « tester » sa disponibilité dans cette mission.

Timeout SSH connexion10s, keepalive5s×2, tâche complète45s ; EXE OFFLINE15s, kill
de ce seul processus sur dépassement. Pas de relance automatique. Une coupure
SSH/timeout ne démontre pas l'arrêt distant : analyser l'état avant toute action
suivante. Les tests unitaires injectent panne réseau/client/timeout ; le kill
Windows sous driver défaillant n'est pas qualifié (driver jamais ouvert).
Codes : retour distant préservé,255 erreur SSH/état inconnu,124 timeout local,
127 client absent,2 arguments/dossier existant. Logs stdout/stderr binaires et
result.json hors Git ; le code 0 global n'exonère pas de lire les sorties réelles.
Le script est envoyé via stdin et un bootstrap EncodedCommand court ; cela évite
la limite de longueur Windows sans modifier de politique PowerShell ou SSH.

Le paquet A existait déjà sur HP : **aucun transfert nécessaire ni effectué**.
Ne pas recopier le paquet ou rejouer M014-C. Un futur transfert D, si nécessaire,
devra utiliser un dossier neuf, SCP/SFTP non interactif avec clé hôte vérifiée,
puis contrôler le hash ZIP/manifeste/tous fichiers avant extraction/exécution ;
ce chemin de transfert n'est pas qualifié par SSH-HP-001.

## Limites et retrait

SSH fonctionne avec un compte administrateur ; `sshd -T` annonce globalement
PasswordAuthentication=yes/PubkeyAuthentication=yes. Aucun service, compte,
pare-feu, clé, règle, profil ou droit n'est modifié. Réduction ultérieure proposée
sous décision dédiée : compte standard limité aux artefacts/tests, authentification
par clé seulement et restriction réseau privé après contrôle des besoins réels.
La portée effective du pare-feu et des clauses Match reste à auditer.

Pas d'essai COM7/RS485, reset, flash ou GDB autorisé dans cette mission. Pour une
future campagne physique il faut un contrat spécifique, une revue ChatGPT, un
nouveau jeton/journal exclusif, une borne de requêtes/timeouts, un maître exclusif
et une cible compatible ; aucun GDB pendant trafic. M014-D reste BLOCKED sur
production/B0/B1. Les jetons C/I sont consommés ; ne jamais les relancer.
L'accès SSH et un succès OFFLINE ne qualifient ni Modbus physique ni M014 globale.

Pour retirer l'outil après arrivée du dock : cesser d'appeler ces scripts ; aucun
agent/service/tâche planifiée ou fichier script n'a été installé sur HP. Archiver
les preuves locales. Le retrait de l'alias/clé dédiée ou du service SSH existants
relève d'une décision utilisateur séparée ; ne pas supprimer des clés partagées
ou arrêter des services automatiquement.

## Gouvernance opérationnelle (M014-E)

T1000 Windows héberge le clientSSH et son alias/clé ; WSL Debian conserve le dépôt
canonique et les builds, Codex pilote ces scripts versionnés depuis WSL. HP Windows
PowerShell héberge seulement la supervision/USB-RS485. Ne pas installer un second
Codex ou migrer le dépôt vers HP. Le dock futur n'est pas requis pour OFFLINE.

Les autorisations viennent du contrat actif GitHub/AGENTS, jamais de l'existence
technique de SSH. La boucle reste ChatGPT→GitHub→Codex→GitHub→ChatGPT : mission
READY figée, inspecter HEAD/status/permissions, journaux/preuves horsGit, commit/push
normal autorisés puis relecture distante du rapport. Aucun envoi HP automatique
ou extension des privilèges par une commande générique. Un seul pilote sur main.

Preuves SSH-HP-001 désormais immuables dans
[le rapport archivé](../../../Modbus%20RTU/00_gouvernance/Echanges_Codex/archives/TR2-20261011-SSH-HP-001/RAPPORT.md).
La mission M014-E autorise seulement des tests SSH OFFLINE utiles, interdit COM7/
transmission/flash/debug/reset/médias physiques. Les succès OFFLINE/hash/retours
observés ne qualifient aucune transmission physique. Les décisions de production
sont suivies dans [le document M014-E](../../../Modbus%20RTU/00_gouvernance/PROVISIONING_PRODUCTION_M014_E.md).
