# Démonstration opérateur LIVE — préparation révision2

Composition diagnostique TR2-001, protocole2 partiel NON-PRODUCTION ; pas de qualification industrielle. Firmware autonome `TR2_STM32_LIVE_OPERATOR=ON` avec recette R5B : montage réel R2/CRC/identité, acquisition arrêtée au boot, calibration capteur2s, START/STOP explicites. Aucun trigger GDB ni sortie à10min dans ce mode. Les recettes historiques conservent leurs gardes et bornes. Les limites normales des compteurs/journal R2 et garde-fous FIFO/FRAM restent actifs ; pas de RENEW automatique. Le buffer fini de traces de maintenance du banc ne constitue pas une autorité d'admission pour ce mode persistant ; l'ownership et les bornes physiques d'écriture LIVE restent inchangés.

Service Windows portable, même lecteur strict et SQLite vivante que UX-L2. `--seconds 0` signifie jusqu'à arrêt explicite ; les anciennes durées1..600 restent utilisables pour essais bornés. Le service d'observation et le Web restent readonly. Les lanceurs START/STOP utilisent un outil opérateur distinct avec le transport NModbus existant et le vrai moteur R2 cible, sans concurrence : demande de pause avec token, acquittement après fermeture du handle, ouverture exclusive, lecture B0/B3/B5 courante, allocation monotone d'identité conservée sur disque avant submit, requête canonique, attente du terminal EXACT epoch/tx/code et effet B3, fermeture avant reprise du poller. Aucune répétition automatique après erreur ; aucune réutilisation5/12–5/13, renouvellement ou écriture FRAM brute.

Le writer S7 Web existant cible un autre profil complet et n'est pas raccordé implicitement au profil diagnostique V1.1 partiel. Ce lot privilégie les lanceurs explicites éprouvables sans modifier cette autorité ; une commande Web intégrée et l'ergonomie relèvent du lot2 à cadrer. La fiche LIVE reste clairement readonly, aucun faux bouton START/STOP.

Répertoire préparé : `C:\Users\lolo\AppData\Local\MSM\TR2-LIVE-DEMO-20261019\`. `app\` contient la publication win-x64 autonome, pas besoin de compiler ou d'utiliser Git. `data\live.db` est la base de démo ; chaque lancement crée une nouvelle trace série et un journal service. `data\transactions.json` est le ledger opérateur ; ne pas le supprimer, le recopier en arrière ou le modifier. `connection-control` est réservé au partage COM ; ne pas l'éditer pendant une commande.

Ordre opérateur prévu, à qualifier sur le matériel avant statut final :

1. Double-cliquer `LANCER_SERVICE.cmd`, garder sa fenêtre ouverte. Port COM4/1152008E1, écoute locale5100, aucun autre maître.
2. Double-cliquer `OUVRIR_EDGE.cmd` : `http://127.0.0.1:5100/live-device.html?deviceId=1&view=vibration` ; parc `/live-fleet.html`.
3. Double-cliquer `START.cmd`, attendre `terminal_and_effect_observed` et code retour0 avant de considérer l'acquisition effective. Le service est temporairement suspendu pour libérer COM ; cela crée des observations de perte explicites, pas des mesures reconstruites.
4. Observer les courbes et les réceptions PC renouvelées. Aucune durée d'arrêt imposée. UTC de fenêtre/run/config restent inconnus sur le fil diagnostique.
5. Double-cliquer `STOP.cmd` et attendre le terminal ET HELD (historiquement30–60s). Ne pas conclure à l'arrêt à la seule demande/acceptation. Redémarrer via START si souhaité après STOP terminal, avec une nouvelle identité.
6. Après STOP confirmé, `ARRETER_SERVICE.cmd` : demande traitée uniquement quand le prochain B3 connecté confirme acquisition arrêtée. Attendre fermeture du service ; fermer les fenêtres opérateur. Ne pas tuer le service pendant une acquisition ou commande. Le firmware reste LIVE en Flash.

En cas d'erreur : aucune répétition aveugle ; conserver les journaux, l'identité affichée et la base. Si cible en acquisition, contrôle STOP/diagnostic prioritaire ; la fermeture du service seule ne commande pas STOP. Une coupure au milieu d'une transaction n'est pas qualifiée : l'image échoue de manière fermée si recovery actif/indéterminé, sans rejouer un effet. La restauration M016 est une récupération conditionnelle, pas la fin normale de cette démo.

Les chemins, lanceurs et résultats effectifs seront consignés dans le rapport physique. Ce document préparatoire ne vaut pas preuve d'exécution sur carte ni d'endurance.

## Résultat effectif du 19/10 — BLOCKED Windows

Code préparé `c36c9718a75e6b40c45707a482e8db836296cab2`. Cross-build canonique et image spéciale réussis ;350 tests .NET hors Service réussis, puis238 Service finaux (dont allocation/terminalité/handoff simulé). Smoke Linux sans matériel : durée0 jusqu'à annulation, pause tokenisée/acquittement et reprise/fermeture propres. Ces résultats ne qualifient pas le fonctionnement physique.

Le smoke **Windows sur COM99 inexistant** échoue avant ouverture série : FileLoadException0x800711C7, Code Integrity3033/3077, politique `{0283ac0f-fff1-49ae-ada1-8a933130cad6}`, DLL `app\TR2.Domain.dll`, SHA-256 `43feefed9b8c7dfd2917051fa9363aea27b760e7ad2a897cd1005412762f0514`. Package local vérifié382 fichiers, publication win-x64 autonome ; aucune politique de sécurité, ADS ou protection modifiée, aucun essai de reconditionnement pour éviter le contrôle. La source Domain est inchangée depuis R5-B2 ; cela n'autorise ni ne prouve la confiance Windows de ce nouveau binaire.

L'utilisateur demande de publier le blocage et les acquis. **Aucun flash, reset, START/STOP physique ou ouverture COM4 n'a été réalisé**. Préflight frais10:04:25UTC : bonne NUCLEO/UID/ST-LINK, Flash2Mio M016 c5bc…f5b26 et FRAM262144o2b501a…7dd84 exactes, WHO7B/capteur arrêté. L'image LIVE n'est donc pas sur la carte ; elle ne peut pas encore être utilisée pour la démo. M016 n'a pas été restauré : il n'avait pas été remplacé.

Reprise : faire autoriser **cette publication Windows** par le mécanisme de confiance prévu sur le T1000. Aucun geste sur la carte ne lève ce blocage. Reprendre ensuite un smoke Windows non matériel, vérifier état cible/FRAM selon temps écoulé, flasher l'image LIVE identifiée, qualifier premier START/STOP, boot autonome, second cycle prolongé surveillé et Edge vivant ; laisser LIVE flashé et STOP final si sain. Ne pas relancer les tests réussis sur code inchangé ou les scripts R5-B2 à clés consommées.
