---
mission_id: TR2-20261011-M013-F-PHYSICAL-RESPONSE-FORENSICS
status: READY
base_ref: main
author: ChatGPT
target_branch: main
allow_commit: true
allow_push: true
allow_flash: false
allow_debug: true
---

# M013-F — Analyser la première réponse RTU physique observée sur HP

## Fait utilisateur nouveau et non reproductible sans mission ultérieure
Après GO M013-E, l'utilisateur a lancé **une seule fois** sur HP `& "$HOME\\Send-Tr2RxProbe.ps1" -Transmit`, COM7 115200 8E1. Sortie exacte :
```
Request: 01 03 03 E8 00 01 04 7A; COM7 115200 8E1; CRC/self-check OK
Observed PC RX (may be echo): 01-03-02-00-00-B8-44
```
Cette séquence de 7 octets est exactement la réponse FC03/1000/1 UNKNOWN0 attendue, avec CRC Modbus valide, **mais le script avertit may be echo**. Ne pas déclarer TX physique ou bidirectionnel définitivement qualifiés sans discrimination par preuves. Aucun nouvel essai HP, aucune nouvelle émission RS485, aucun reset/flash autorisés dans F.

## Objectif
Contrôle indépendant de la réponse et des compteurs STM32 instrumentés M013-E, identification raisonnable de l'origine des 7 octets, sans perturber l'état cible ni altérer les preuves. Relire AGENTS.md, rapport E, diagnostic D, sources, script HP si disponible dans dépôt, documentation gouvernance. Vérifier HEAD distant et état local/stashes. Vérifier CRC réponse et distinguer explicitement réponse attendue, écho brut de requête, écho transformé, autres maîtres et limites de preuve.

## Exécution
1. Préserver état et preuves E, vérifier images/ELF/symboles exacts et dossier `/home/lolo/dev/msm/reports/TR2-mission013-E-20261011T160141Z`. Préférer les preuves déjà présentes et l'analyse statique.
2. Si nécessaire et sûr, effectuer **uniquement GDB/ST-LINK sans CubeProgrammer HOTPLUG**, lecture passive RAM/observateur IRQ→HAL→RTU→TX/TC, UART, NVIC, PG4, erreurs et compteurs. Ne pas reset/halt prolongé, ne pas flasher, ne pas ouvrir COM7, ne pas transmettre ; GDB detach et laisser cible running. Préserver snapshots bruts et limites d'observabilité. Si l'accès debug altère l'état, interrompre et documenter.
3. Comparer deltas observateur avant probe (compteurs 0) et après probe, notamment réception 8 octets, CRC, unité1, FC03, préparation réponse 7 octets, démarrage TX, TC callback, retour RX, erreur/timeout. Vérifier éventuelles limites de capture temporelle DE/dernier stopbit et distinguer observation indirecte d'une capture physique.
4. Produire verdict nuancé : ce qui est démontré côté HP et MCU, ce qui demeure non qualifié. Si réponse et compteurs concordent, proposer prochaine tranche de qualification temporelle / robustesse, sans la lancer.
5. Mettre à jour DERNIER_RAPPORT.md et ETAT_COURANT_TR2.md ; commit/push normal main, relecture distante. Ne modifier ni V1 ni firmware pour ce diagnostic sauf justification expresse et sans changer l'image cible. Respecter AGENTS.md, conserver stashes et backups. Aucune nouvelle manipulation utilisateur demandée.

## Critères
Preuves traçables de la réponse observée, CRC vérifié, instrumentation STM32 interprétée sans extrapolation, zéro nouveau trafic HP, zéro flash/reset, zéro CubeProgrammer HOTPLUG ; rapport vérifié sur main. Les autorisations `allow_flash:false` et `allow_debug:true` n'autorisent que les lectures passives nécessaires.
