# Preuves TR2-20261019-LIVE-COM4-LOSS-DIAG

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
