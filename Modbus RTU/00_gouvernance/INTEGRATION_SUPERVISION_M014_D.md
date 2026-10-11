# M014-D — écarts, raccordements validés et reprise

11 octobre 2026. **M014 EN COURS / BLOQUÉ pour la qualification opérationnelle
physique.** Les tranches PC ci-dessous sont validées logiciel ; le diagnostic
M014-C reste acquis pour un seul échange et sa commande est consommée.
La roadmap conserve ses jalons : aucune qualification M014 globale anticipée.

## Matrice de dépendances réelle

| Fonction / autorité | Développé et testé | Preuve physique disponible | Écart / critère de passage |
|---|---|---|---|
| UART/RTU/CRC/TX diagnostic STM32 | serveur RTU et HAL, observateur RAM | M013-I/J et unique .NET M014-C corroborés | timing DE/TC/stopbit/retourRX/endurance restent ouverts |
| Diagnostic FC03/1000/1 | stm32_modbus_diagnostic.c, UNKNOWN réel d'initialisation incomplète | image E exacte, réponse nominale .NET rapportée et deltas MCU C | ne fournit ni B0 ni B1 complet ; ne l'élargir avec des zéros fictifs |
| Serveur ModbusSystemServer | projections V1 depuis vrai SystemRuntime, tests RTU/PDU hôte | pas production sur carte | M014-D vérifie B0/21 et B1/20 complets, CRC, actualisation et refus04 puis retour source |
| ProductionApplication | runtime possédé, dispatcher/staging/workflow, boot/readiness réels | simulé hôte, compilé ARM | services média/capteur/backend doivent être durables ; pas réutiliser fixtures harness |
| Binding STM32 | stm32_modbus_application_bind / runtime statique | faible stm32_production_binding_acquire refuse NOT_AVAILABLE | fournir autorités carte qualifiées et durables avant toute préparation |
| Média FRAM transactionnel | geometry_validate/recover, EMPTY refusé | chaîne stockage précédente avec ses limites, pas provisioning production courant démontré | géométrie réelle, image VALID et décision humaine de provisioning ; pas format implicite |
| Campagnes microSD | Stm32SdmmcBulkMedia + CampaignDataStoreBulk + tests system_runtime_bulk | E4 ouvert ; microSD historiquement absente | composition durable dédiée/allocation/géométrie et arbitrage écritures harness ; ne pas substituer backend historique FRAM |
| Identité B0 et adresse | ModbusBlock0Image/B0Reader/SessionFactory | identité production non provisionnée | DeviceId stable/unique/persistant, métadonnées/version explicites ; ne pas dériver silencieusement UID/adresse |
| Supervision S0–S7 | transport/NModbus/SQLite/scheduler/polling/web développés, fakes | diagnostic .NET C seulement | B0 compatible réel puis blocs périodiques et perte/reprise physiques à observer |
| Contrôle PC borné D | --system-check, OFFLINE défaut, B0→B1, maximum2 requêtes, journal exclusif | pas lancé HP | NO-GO tant que vrai B0/B1 absent de cible ; syntaxe/code/tests ne qualifient pas driver HP |
| Perte/reprise PC D | invalidation télémétrie de tout bus, B0 neuf à reconnexion, identité/version revérifiées | simulé avec vrai runtime PC/SQLite | perte physique puis reprise à qualifier sous plan borné et revue ChatGPT |

Références de code : `05_Firmware/platform/stm32/stm32_modbus_application.c`,
`stm32_production_runtime.c`, `stm32_modbus_diagnostic.c`,
`src/application/modbus_system_server.c`, `production_application.c` ;
`Supervision TR2/src/TR2.Protocol/B0Reader.cs`, `B1Reader.cs`,
`TR2.Application/PollingExecutor.cs`, `TR2.Supervision.Service/PhysicalPollingWorkRunner.cs`,
`SerialBusRecoveryCoordinator.cs`, `SerialBusReconnectScheduler.cs`, `SupervisionPollingLoop.cs`.
Les blocs opérationnels existants FAST=B1/B3/B5, MEDIUM=B2/B7, SLOW=B4/B6 exigent
plus que B0/B1. Une réussite de deux blocs n'autorise pas le polling complet.

## Tranches logicielles livrées

1. En perte I/O d'un bus partagé, tous ses appareils identifiés passent leurs
   snapshots en indisponible avant oubli de session et fermeture du driver.
   Dernières valeurs et timestamps conservés, y compris si Dispose échoue.
   Reconnexion ne réactive aucune valeur ; nouvelle B0 requise, nouvelle identité
   réellement lue ; nouveau polling nécessaire pour rendre les données disponibles.
2. Notification de session compatible seulement pour B0 Compatible : une version
   incompatible ne déclenche plus ce signal vers le runtime opérationnel.
3. Nouvelle entrée `--system-check` isolée avant config/runtime. Elle réutilise
   les validations des paramètres diagnostic ; version protocole explicitement
   fournie, pas de DeviceId/READY supposé. B0 quantité21 exactement puis B1 quantité20
   exactement seulement après compatibilité. Une connexion, deux lectures maximum,
   aucun retry/reconnexion/polling/écriture ; code0 après fermeture et journal résultat.
   UNKNOWN ou autre SystemStatus sont décodés sans leur attribuer un READY fictif.
4. Tests firmware RTU du serveur réel sur runtime hôte : B0/21 et B1/20 répétés,
   image B1 actualisée, retrait source→exception04, retour source→lecture complète.
   Identité et services de test sont explicitement des fixtures hôte.

Tests .NET : perte bus multi-appareils/fermeture échouée ; reconnexion même identité,
identité différente, version incompatible, données indisponibles jusqu'à lecture
fraîche ; notification compatible ; contrôle B0/B1 OFFLINE/config invalide/log existant/
cancellation/mismatch/taille/timeout/I/O/exception/fermeture sans retry. Vrai NModbus
sur flux mémoire : requêtes `0103000000158405` puis `010303E80014C5B5`, registres21/20,
B1 CRC invalide rejeté sans nouvel envoi. Pas capture physique ni port réel.

## Blocage structurant et décisions attendues

Le port faible de carte n'est pas un simple oubli à remplacer par une fixture.
Il refuse précisément les autorités non provisionnées. Une activation aujourd'hui
nécessiterait des décisions non documentées sur l'identité persistante et le
provisioning FRAM/SD, ainsi que le backend/allocation exclusif au runtime.
AGENTS §8 impose de signaler cette décision structurante ; §9 ne permet pas de
transformer allow_flash en autorisation d'écriture arbitraire non volatile.
Ne pas contourner avec DeviceId=42, UID réduit, image EMPTY formatée, backend
hôte/historique ou drapeau READY. Ne pas lancer le harness E1/E4 sur des médias
production sans arbitrage de propriété. Aucun défaut matériel nouveau n'est déduit.

Décisions ChatGPT/utilisateur : source et politique d'identité/adresse/version,
géométrie/média autorisés avec sauvegardes et procédure de provisioning dédiée,
backend SD et allocation/config metadata durables, séparation boot production/harness.
Après ces décisions : implémenter la composition carte explicite, tester échecs
sans média/EMPTY/identité absente, compiler, puis préparer sauvegarde/restauration
et flash standard sous les garde-fous AGENTS. Ne pas changer câblage ou gels.
Les acquis électriques ne sont pas à refaire ; l'insertion éventuelle de média
reste une action humaine cadrée, pas une instruction à exécuter maintenant.

## Paquet D et procédure HP minimale

Paquet autonome préparé T1000, non transféré/exécuté HP :
`/home/lolo/dev/msm/reports/TR2-mission014-D-20261011T182756Z/TR2-M014-D-win-x64.zip`.
Code évalué `ccc20526f478ab47c097528a0af33d760f33f22b`,370 fichiers + manifesteD.
ZIP50094222octets SHA256 `ecd7fa606a16258afcb087b388fd7a35382a5bccad8e0b34c9f87aff25a35fc2` ;
EXE SHA256 `9b5d247be3f86ef606352f3f8a9faa74cacd5cae9a39cb314f6e93edf62bf6d6` ;
manifeste SHA256 `a3d60750443517ab3238de3c759ef3f04c6dee322ee2d96bb9e21c20dc72577c`.
Originals A/C conservés ; binaire horsGit, aucune URL GitHub binaire supposée.

**Aucun essai série HP à exécuter maintenant.** Après revue ChatGPT, le transfert
et contrôle hors port peuvent utiliser ce nouveau paquet, dans un dossier neuf
D sans réutiliser marqueur C. Vérifier ZIP/EXE/manifeste370, modèle HP/PnP COM7,
aucun maître concurrent, puis cette invocation OFFLINE :

```powershell
& $exe --system-check --protocol-version 1 --port COM7 --baud 115200 --parity Even --timeout-ms 1000
```

`$exe` devra être le chemin vérifié du nouveau paquet D. Version1 est la politique
PC explicite S5, pas un provisioning implicite du firmware. Code0 et message
OFFLINE seulement attendus : pas port, émission ou journal. Ne pas substituer le
paquet A qui ne connaît pas cette entrée. Pas installation SDK/ExecutionPolicy.

Après vraie cible B0/B1 identifiée, baseline hors trafic et revue ChatGPT, le futur
contrôle physique ajoutera `--read-once --log <nouveau journal D exclusif>` :
maximum une FC03/0/21 et une FC03/1000/20, stop immédiat toute anomalie, aucune
relance. Sa commande complète avec contrôle hash/verrou mission neuf devra être
figée au moment du GO, **pas de commande de transmission prête à coller ici**.
Conserver journal exact/hash/code/transcript ; CRC vérifié par NModbus, journal
registres décodés distinct de capture fil. GDB seulement avant/après, jamais pendant.

Qualification périodique/reprise ultérieure : définir durée et nombre de lectures
maximum, groupes effectivement disponibles, seuils perte et conditions d'arrêt,
aucune écriture B5. Une perte de liaison nécessitant un geste physique doit être
faite par l'utilisateur après cadrage ; le scheduler logiciel existant se reconnecte
à cadence explicite mais son loop --config est continu : ne pas l'utiliser comme
campagne bornée sans définir son arrêt. Ne jamais confondre reprise simulée et HP.

## Point de reprise sans refaire le travail

- Relire rapport D et Git courant, conserver deux stashes005/002 et archivesC.
- Réutiliser code/tests/paquet ci-dessus, pas rebuild si source inchangée.
- Preuves D : `build.log`, `recovery-tests.log`, `system-tests.log`, `full-tests/`
  et TRX, `recovery-final-tests/`, `firmware-validation.log`, `system-offline.log`,
  `publish.log`, `test-summary.json`, `package-summary.json`, manifestes et relectures.
- Pas de fichier source inachevé laissé en attente ; les tranches livrées sont
  testées/commitées. Les travaux restants sont la composition carte réelle et la
  qualification HP bloquées ci-dessus, pas un correctif à deviner.
- Diagnostic E restaurable conservé : BIN49200 SHAdc6ac75f…/ELF4c496149… et
  sauvegardes antérieures horsGit. Avant tout futur flash, faire sauvegarde réelle
  cible et plan de restauration contrôlé ; aucun nouveau flash/debug D réalisé.
- M014 global/J2 opérationnel restent EN COURS ; DE/TC/timing/robustesse/J1global/
  acquisition/production/E4 ouverts. M014-C reste acquis dans sa portée bornée,
  avec journal HP intégral non fourni à Codex : hash annoncé, pas recalculé.
