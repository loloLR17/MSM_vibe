---
mission_id: TR2-20261012-M014-H-PRODUCTION-RESUME
status: BLOCKED
base_ref: main
base_sha: 30f4663c7eece9ffb66740da1466c0ae88432f5a
initial_head: b3a4ac7125b59f57979abd0a1458c11e52515504
result_sha: null
created_at_utc: 2026-10-12T07:46:16.958236+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# M014-H — Point avant provisioning physique

M014 IN PROGRESS. Rapport incrémental de conservation ; BLOCKED couvre les
critères physiques et la relecture distante encore à obtenir, pas une demande
d'approbation utilisateur. Contrat actif inchangé, archives G exactes depuis
30f4663 ; deux stashes conservés. Main initial b3a4ac7, arbre propre.

Acquis G réutilisés : 113 tests firmware + cross, 539 .NET non relancés sur code
inchangé. UID/ST-LINK/probe courant vérifiés via GDB seul ; Flash2Mio et RAM768K
préservées, Flash courante comparée identique au dump G. Première commande dump
GDB UNC échouée ; corrigée avec chemin forward-slash qualifié, aucun reset/write.

Livrés : manifeste H opérationnel pour supervision dégradée, générateur registre
vers identité/header et B0 attendu, strong binding durable STM32, profil
manufacturing explicitement déclenché et profil production hors harness. Aucune
instruction HAL SD read/write liée dans les images. Aucune allocation SD et
aucune modification de sa partition. Identité versions attribuées hw1/fw0.14.8/
proto1/caps0xC ; origine projet documentée, pas mesure ni fixture.

La configuration persistée est validée **structurellement seulement**, sans
prétendre à une capacité usable : boot normal environment=false/capacité0,
configuration UNAVAILABLE, disponibilité stockagefalse/READYfalse. Les fonctions
campagnes indisponibles refusent et ne remplacent pas le backend bulk par FRAM.
Cette réserve est explicite dans manifeste et procédure ; pas configuration
opérationnelle ou acquisition qualifiée.

Résultats nouveaux : deux cross-builds dédiés réussis ; six tests du générateur
réussis ; répétition offline sur copie du backup FRAM réel réussie : refus sans
autorisation/mauvaisUID, générations10→11→12, six writes, autres domaines
inchangés, B0 exact, configuration UNAVAILABLE sans ressource. Compilation du
check hôte initialement échouée pour include ConfigurationStore manquant,
corrigée ; pas assertions affaiblies. Cross-build du profil par défaut requis
par modifications main/deps STM32 ; retour0 obtenu (default-cross-validation.log). La commande tr2_validate.sh se lance depuis la racine Git ; la mention de cwd firmware dans le rapport G était inexacte.

**Aucune écriture FRAM physique encore faite, aucun flash H, aucun COM7/RS485.**
Restent flash manufacturing qualifié, backup FRAM cible byte-equal et trigger
unique, readback complet/boot production, paquetHP OFFLINE puis essais bornés
B0/B1 polling/perte/reprise. Procédure dans COMMISSIONING_PRODUCTION_M014_H.md.
Preuves : /home/lolo/dev/msm/reports/TR2-mission014-H-20261012T073729Z. Résultats G et E conservés,
M013/C/I non rejoués ; limites timing/DE/TC/robustesse/E4 inchangées. Ne pas
annoncer DONE sur compilation ou provisioning seul.

## Intégration concurrente contrôlée

Push82169af refusé après mise à jour ChatGPT43f37db (arbitrage SD/communication). Autorisation utilisateur explicite de réappliquer la tranche dans un worktree isolé puis commit/push normal. Toutes les sources réappliquées sont byte-identical au commit évalué ; aucune validation répétée pour ce déplacement. Dépôt initial et commit82169af préservés ; worktree de publication MSM_vibe-M014-H sur branche locale codex/m014-h-integration, destination explicite main. Contrat clarifié43f37db respecté.

## Provisioning physique réellement obtenu et paquet HP

Source logicielle publiée/relecture intégrale15livrables byte-equal au SHA fa7b464998484b634f4cdc46d9c33e7f2be71a4a (tranche évaluée82169af, mêmes sources). Flash manufacturing NORMAL0x08000000/verify réussi, secteurs0..8 seulement. BIN67028 SHA92df5bd857156cec58d0c375d3fd02a51f9d6b5f7f000a7c65750b0d493e1823, ELF e0491ec95b5ed1d479ce6254cb2ed53773fefc6d2f7a2360b6cf5c085c5b6fb8. Reset GDB seul aprèsCLI ; preflight stage2/result0/génération10/trigger0 ; backup262144 extrait de cible byte-equal à G.

Unique trigger RAM0x4D313448 effectué. Observation résultat stage5/result0/génération12. RAM/51914logique préservées puis reset logiciel pour prouver persistance et obtenir le readback complet : preflight refuse désormais le plan10 périmé (INVALID_STATE/trigger0), aucun retry/écriture. Backup après262144 identique intégralement à simulation offline, V2/génération12 activeB, SHA d732acc557152180fc394097d6a5269ea2f45c1da387c8643f512318adf92fce. Premier contrôle host utilisait à tort offsetimageA pour comparer la logique ; corrigé à activeB lu dans header, comparaison intégrale physique déjà identique. Config seulement structurellement valide, activation stockage encore UNAVAILABLE ; aucune SD accédée.

Paquet autonome HP nouveau transféré/extrait sans overwrite,371fichiers hashes/tailles vérifiés puis commissioning-check OFFLINE exécuté retour0/no port/no transmission. Source PC G inchangée,539tests historiques conservés, pas relancés ; publish Release win-x64 nouveau réussi. ZIP62e5bb63a085366ecfe2671f335f99cd61b022a145d26c5eaf3ca4ec167ca77d, manifeste37ad4597af22b75d09f9e1ba89bacc6c3a3cefb2c8a53e344085b0f689ac2068. OutillageH distinct des tâchesSSH historiques nonphysiques ; mutex, manifests/B0/âgegate, répertoireunique physical-once,280sdeadline,9FC03/retry0.

Profil production qualification avec observer passif et fenêtre silence unique90..180s compilé. Pas encore flashé à ce point, aucune transmissionCOM7. Étape suivante : flash/bootpreflight récent puis une campagne HP, aucun GDB pendant trafic.

## Première campagne physique et correction nécessaire

Premier preflight Physical refusé pour gate temps périmé, avant port et répertoirephysical-once : zéro transmission. Écart horloge HP/T1000 mesuré par SSHreadonly (RTT4.679643s, offsetcentral-2.555168s) ; gate converti avec borneinférieure-4.894989s, âge conservateur0..30s inchangé, aucune horloge modifiée. Nouvelle baseline GDB uptime4477ms, RX/TX0, IRQactives, detach/serveursfermés avant trafic.

Campagne réelle achevée HP08:05:08UTC, retour0,9requêtes/8réponses/un timeout prévu, deux sessions série fermées, retry0. Journal8467octets/31événements récupéré intégralement par SCP et hash HP/local vérifié2E63B72CA75184C7866F52184AE50721748A7B979624B9B36C47EAFD45150DC5. Copies brutes dans preuves/TR2-20261012-M014-H/physical-before-b1-fix.jsonl. Polling3+3/B0neuf/invalidationanciennevaleur et date/réactivationB1frais effectivement observés. GDB APRÈS seulement, sans reset : RXcallback/bytes72, TXstarted/TC8, silenceevents10, erreurs/drop/failsafe0, IRQsaines/filevide.

**Anomalie empêchant la clôture** : uptimeB1 constant1s malgré tick295295ms. Projection boot copiée sans producteur vivant. Correctif minimal : callback refresh_status optionnel sur binding, fourni par composition production ; actualise via agrégateur runtime avant réponse, absent→politique historique inchangée ; retrait explicite B1reste04. Test négatif hôte reproduit uptime figé ; nouvelle assertion d'avance temps sans B5 échoue sur anciencode. Premier essai de refresh inconditionnel incompatible avec testserviceexplicit4, échec conservé ; corrigé en authority opt-in, assertionsanciennes inchangées, pas de contournement. Testciblé réussi puis validationcomplète **113/113 + cross-build** retour0, justifiée par interfacepartagée/cœurmodifiés. Images production permanente et qualification corrigées compilées. PC/G/539tests inchangés nonrelancés, même paquet réutilisable sans rebuild.

M014 demeure IN PROGRESS à ce point : la première campagne prouve transport/perte/reprise mais pas actualisation correcte. Prochaine action : image corrigée, nouveau préflight et campagne bornée distincte justifiée par correction pertinente, pas retry automatique ni replayC/I.

## Préparation de la qualification corrigée

Correctif B1 publié au SHA0b2a901e185c9cbf8c02410bf2a29a0a3a75491e, huitlivrables relus depuisorigin/main byte-equal. Image corrigée qualification BIN143828 SHA089bff0854421c67f4248882470d4b178a1372ab58a074a8989fc9b04a6a5c56 / ELF2b361ae22647174018a4025967ec7364ea46262de9e42862ca35760352ae4c0e flashée/verifyNORMAL aprèsRAMpréservée.

Préparation d'un paquet identique dans un second dossier HP échouée : disqueplein, extractionincomplète et manifesteabsent, aucuneouvertureCOM7. PaquetPCinitial conservé et réutilisé, pasrebuild ni539tests. Seule la copieZIPtemporaire nouvellement créée parCodex a été retirée aprèshashégal àl'originalconservé ;42.38→92.49Mo libres. Dossierpartiel conservé, aucun fichierpréexistant/utilisateur supprimé. Nouveaujournalborné `physical-after-b1-fix` autorisé uniquement pour la correction pertinente0b2a901 ; ancienphysical-once immuable, aucunretryautomatique, mêmebudget9FC03/retry0. OFFLINE du paquetexistant avec nouveauwrapper réussi,371fichiersvérifiés.
