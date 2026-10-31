# Récupération réelle après coupure — preuves compactes

`RECOVERY_RESULTS.json` et `COPIES.json` : fichiers originaux sur carte et copiesPC intégralement hachés ; lecteurstrictv3 inchangé appliqué à tous les records. Le long estCOMPLETE au sens du format, toutes données utiles conformes au digest dans sonfooter. **La référence RAM externe du long a été perdue**, aucun rapprochementRAM indépendant n'est inventé. Le gatecourt dispose de sa référenceRAM antérieure et sa comparaison est exacte.

Les originaux complets résident dans le répertoirelocal indiqué parEXCERPTS.json, ainsi que sur la microSD. Les morceaux `.bin` sont de vrais512octets aux offsets indiqués ; ils ne permettent pas de recalculer le SHA du flux complet. PREVIEW_REAL_SAMPLES.csv décode uniquement ces records ; temps intra-blocinterpolés approximatifs en unitésMCUnominales, pasUTC, countsint16 à244µg/compte nominaux. Qualitéagrégée parrecord, warmup réel11sorties au début. DCconservée, orientationXYZ navire inconnue ; pasdeconformitémétrologique déduite.

Les snapshots `current-*` depost-reset-state sont l'étatWAIT après coupure, **pas les compteurs finaux de l'acquisition**. Les anciens compteurs de latence/file/pile et le digestRAM ne sont pas récupérés. Aucun nouveauflash/reset/START n'a été réalisé. Les SHA des treize historiques sont inchangés.
