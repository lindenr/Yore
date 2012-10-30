#Yore ideas

This is a disorganised set of thoughts on Yore.

<pre>
    Back in the days of Yore, in a land far removed
    from our current understanding of the universe,
     when magic flowed throughout the air as water
     flowed through the sea, and the Gods lived in
      harmony with the people; it was a time when
        anything and everything was possible...
</pre>

##How spellcasting works (will work)

You have belt studded with jewels (maybe 5? maybe longer belts exist but are exponentially rarer/more expensive) in a row. Each jewel has an 'effect'.
When you cast a spell the effect of each jewel in turn is applied to the spell until it is finally cast at your opponent.
Thoughts on what the jewels could do:

 * increase elemental/general damage/defense
 * moves something (eg you/your target)
 * changed attack type
 * add a confusion attack
 * summon something
 * all the above with an added cost (eg HP loss, confusion, etc) depending on how OP it is

Could do it like NH - have a set list of descriptions of jewels ('red', 'black', 'striped', etc), and at game start randomly assign each one to a function
('double fire damage', 'triple damage against undead', etc). How would the player identify the jewels? Naming? Messages (like 'This fire attack feels twice as powerful!')? Monster use?

##Graphics

I think the reason it's laggy is because update_stats is called once per move *per monster*.
So, at the beginning everything is fast; as monsters are generated (which you can't even see) they slow you down.
Evidence: as I was playing every few turns my movements seemed to slow down slightly.
gr_refresh, I suspect, has basically no impact on speed.

I'm not sure how to fix this though; some optimisation in visualise_map is certainly called for.
Maybe check each time all_things is modified if it will affect the screen; then only those squares which have changed need to be gone through in visualise_map.
Alternatively if anything visible has changed call visualise_map normally, otherwise don't call it at all.
Easier, simpler and probably just as quick. Monsters moving out of sight won't impact on speed.
Even cruder: only call update_stats at the end of each turn. I think I'll do that, for now. In fact, it works excellently. Yay :)
